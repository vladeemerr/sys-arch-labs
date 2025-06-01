#include "product.hpp"
#include "userver/formats/bson/document.hpp"
#include "userver/formats/bson/exception.hpp"

#include <optional>

#include <userver/components/component_context.hpp>
#include <userver/formats/bson.hpp>

namespace {

constexpr char collection_name[] = "goods";

formats::bson::Document idFilter(const std::string_view id) {
	return formats::bson::MakeDoc("_id", formats::bson::Oid(id));
}

} // namespace

ProductStorage::ProductStorage(const components::ComponentConfig &config,
                               const components::ComponentContext &context)
: components::ComponentBase(config, context),
  storage_(context.FindComponent<components::Mongo>("shop-database").GetPool()) {}

std::string ProductStorage::addProduct(const ProductInfo &info) {
	using formats::bson::MakeDoc;

	formats::bson::Oid oid = formats::bson::Oid::MakeMinimalFor(
		std::chrono::system_clock::now());
	
	auto collection = storage_->GetCollection(collection_name);
	auto result = collection.InsertOne(MakeDoc(
		"_id", oid,
		"name", info.name,
		"description", info.description,
		"quantity", info.quantity,
		"price", info.price
	));

	if (result.ServerErrors()[0])
		return "";

	return oid.ToString();
}

std::optional<Product> ProductStorage::getProduct(const std::string &id) const {
	using formats::bson::MakeDoc;
	auto collection = storage_->GetCollection(collection_name);

	formats::bson::Document filter;
	try { 
		filter = idFilter(id);
	} catch (formats::bson::BsonException &) {
		return std::nullopt;
	}
	
	auto result = collection.FindOne(filter);

	if (!result.has_value())
		return std::nullopt;

	const auto &doc = result.value();

	return std::make_optional(Product{
		doc["_id"].As<formats::bson::Oid>().ToString(),
		ProductInfo{
			.name = doc["name"].As<std::string>(),
			.description = doc["description"].As<std::string>(),
			.quantity = doc["quantity"].As<int32_t>(),
			.price = doc["price"].As<int32_t>(),
		}
	});
}

std::vector<Product> ProductStorage::getProducts(const std::string &filter) const {
	using formats::bson::MakeDoc;
	auto collection = storage_->GetCollection(collection_name);
	auto cursor = collection.Find(MakeDoc("name", MakeDoc(
		"$regex", filter + ".*",
		"$options", "i"
	)));

	if (!cursor)
		return {};

	std::vector<Product> result;

	for (const auto &doc : cursor) {
		result.emplace_back(Product{
			.id = doc["_id"].As<formats::bson::Oid>().ToString(),
			.info = {
				.name = doc["name"].As<std::string>(),
				.description = doc["description"].As<std::string>(),
				.quantity = doc["quantity"].As<int32_t>(),
				.price = doc["price"].As<int32_t>(),
			},
		});
	}

	return result;
}

std::optional<Product> ProductStorage::updateProduct(const std::string &id,
                                                     const ProductInfo &info) {
	using formats::bson::MakeDoc;
	auto collection = storage_->GetCollection(collection_name);

	auto result = collection.UpdateOne(
		MakeDoc("_id", formats::bson::Oid(id)),
		MakeDoc("$set", MakeDoc(
			"name", info.name,
			"description", info.description,
			"quantity", info.quantity,
			"price", info.price
		))
	);

	if (result.ServerErrors()[0])
		return std::nullopt;

	return std::make_optional(Product{id, info});
}

bool ProductStorage::removeProduct(const std::string &id) {
	using formats::bson::MakeDoc;
	auto collection = storage_->GetCollection(collection_name);

	formats::bson::Document filter;
	try { 
		filter = idFilter(id);
	} catch (formats::bson::BsonException &) {
		return false;
	}

	auto result = collection.DeleteOne(filter);

	if (result.ServerErrors()[0])
		return false;

	return true;
}

ProductInfo Parse(const formats::json::Value &value, formats::parse::To<ProductInfo>) {
	return ProductInfo{
		.name = value["name"].As<std::string>(),
		.description = value["description"].As<std::string>(),
		.quantity = value["quantity"].As<int32_t>(),
		.price = value["price"].As<int32_t>(),
	};
}

formats::json::Value Serialize(const Product &product,
                               formats::serialize::To<formats::json::Value>) {
	formats::json::ValueBuilder result;
	result["id"] = product.id;
	result["name"] = product.info.name;
	result["description"] = product.info.description;
	result["quantity"] = product.info.quantity;
	result["price"] = product.info.price;
	return result.ExtractValue();
}

formats::bson::Document Serialize(const Product &product,
                                  formats::serialize::To<formats::bson::Document>) {
	return formats::bson::MakeDoc(
		"_id", formats::bson::Oid(product.id),
		"name", product.info.name,
		"description", product.info.description,
		"quantity", product.info.quantity,
		"price", product.info.price
	);
}
