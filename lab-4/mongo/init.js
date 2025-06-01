db.goods.drop();
db.createCollection("goods");

db.goods.insertMany([
	{
		name: "Bottle of Milk (0.5l)",
		description: "Fresh, makes you grow!",
		quantity: 100,
		price: 5
	},
	{
		name: "Box of chicken eggs (10 pcs.)",
		description: "May hatch into a yellow birb",
		quantity: 50,
		price: 7
	},
	{
		name: "Loaf of bread",
		description: "Smells awesome!",
		quantity: 20,
		price: 3
	},
	{
		name: "Tomato",
		description: "Sweet red grapes?",
		quantity: 60,
		price: 2
	},
	{
		name: "Cucumber",
		description: "May scare your cat away!",
		quantity: 50,
		price: 3
	}
]);

db.goods.createIndex({"name": 1})
