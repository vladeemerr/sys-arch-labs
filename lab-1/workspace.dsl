workspace "Online shop" {
	description "Yet another online shop system"
	!identifiers hierarchical

	model {
		group "Users" {
			guest = person "Guest" "Can view online shop goods and register an account"
			customer = person "Customer" "Can view online shop goods, manages a cart, makes orders"
			admin = person "Administrator" "Can create or modify shop offers, manages registered users"
		}

		online_shop = softwareSystem "Online shop" {
			description "Allows guests and customers to view and order store goods, and admins to update store offers"

			user_database = container "User database" {
				tags "database"
				description "Stores information about registered cusomers"
				technology "PostgreSQL"
			}

			user_service = container "User service" {
				description "Handles customer information query and authentication"
				technology "C++"

				-> user_database "Reads or updates customer information" "SQL"
			}

			cart_database = container "Cart database" {
				tags "database"
				description "Stores cart information for each customer"
				technology "In-memory"
			}

			shop_database = container "Shop database" {
				tags "database"
				description "Stores information about available shop goods"
				technology "Mongo"
			}

			shop_service = container "Shop service" {
				description "Handles shop item queries"
				technology "C++"

				-> shop_database "Queries and update information about available shop goods" "JS"
			}

			cart_service = container "Cart service" {
				description "Handles customer cart management"
				technology "C++"

				-> cart_database "Reads and updates cart information" "Native"
				-> shop_service "Makes queries to get available shop information" "HTTP"
			}

			guest -> user_service "Registers an account and authenticates" "HTTP"
			guest -> shop_service "Seeks and views store goods" "HTTP"
			customer -> user_service "Can view, update and remove user information" "HTTP"
			customer -> shop_service "Can view goods" "HTTP"
			customer -> cart_service "Maintains a cart (add, view, remove)" "HTTP"
			admin -> user_service "Maintains users" "HTTP"
			admin -> shop_service "Adds, updates and removes shop items" "HTTP"
			admin -> cart_service "Maintains carts" "HTTP"
		}
	}

	views {
		themes default

		systemContext online_shop {
			include *
			autoLayout
		}

		container online_shop {
			include *
			autoLayout
		}

		dynamic online_shop "user-registration" {
			autoLayout lr
			description "Customer registration process"

			guest -> online_shop.user_service "Guest issues POST HTTP request with required user info"
			online_shop.user_service -> online_shop.user_database "Inserts a new customer entry into"
		}

		dynamic online_shop "user-info-query" {
			autoLayout lr
			description "User information query process"

			admin -> online_shop.user_service "Admin issues GET HTTP request with a login in query path"
			online_shop.user_service -> online_shop.user_database "Makes a query by login to"
		}

		dynamic online_shop "product-creation" {
			autoLayout lr
			description "Product creation process"

			admin -> online_shop.shop_service "Admin issues POST HTTP request with required product info"
			online_shop.shop_service -> online_shop.shop_database "Insert a new product entry into"
		}

		dynamic online_shop "product-listing" {
			autoLayout lr
			description "Product listing process"

			{ /* Parallel sequences' syntax sucks */
				{
					guest -> online_shop.shop_service "Guest issues GET HTTP request"
				}
				{
					customer -> online_shop.shop_service "Customer issues GET HTTP request"
				}
			}
			online_shop.shop_service -> online_shop.shop_database "Makes a product query from"
		}

		dynamic online_shop "add-product-to-cart" {
			autoLayout lr
			description "Adding a product to cart process"

			customer -> online_shop.cart_service "Customer issues POST HTTP request with product ID and quantity"
			online_shop.cart_service -> online_shop.shop_service "Service issues GET HTTP request with product ID to confirm its existence"
			online_shop.shop_service -> online_shop.shop_database "Makes a product query from"
			online_shop.cart_service -> online_shop.cart_database "Inserts new cart entry for user into"
		}

		dynamic online_shop "list-cart" {
			autoLayout lr
			description "Listing a cart contents process"

			customer -> online_shop.cart_service "Customer issues GET HTTP request"
			online_shop.cart_service -> online_shop.cart_database "Queries user's cart entries from"
		}

		styles {
			element database {
				shape cylinder
			}
		}
	}

	configuration {
		scope softwaresystem
	}
}
