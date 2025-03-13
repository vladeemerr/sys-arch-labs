workspace "Online shop" {
	description "Yet another online shop system"
	!identifiers hierarchical

	model {
		group "Users" {
			guest = person "Guest" "Can view online shop goods and register an account"
			customer = person "Customer" "Can view online shop goods, manages a cart, makes orders"
			admin = person "Administrator" "Can create or modify shop offers, manages registered users"
		}

		group "External systems" {
			payment_system = softwareSystem "Payment system" {
				description "Handles payment transactions"
				tags "external"
			}

			warehouse_system = softwareSystem "Warehouse management system" {
				description "Stores information about available store goods and manages their availability"
				tags "external"
			}

			notification_system = softwareSystem "Notification system" {
				description "Handles Email/SMS/Push notifications"
				tags "external"

				-> customer "Notifies of order status"
	 		}
		}

		online_shop = softwareSystem "Online shop" {
			description "Allows guests and customers to view and order store goods, and admins to update store offers"

			-> payment_system "Makes payment using"
			-> warehouse_system "Gets information about available goods, updates store items"
			-> notification_system "Forwards order status via"

			client_webapp = container "Client web application" {
				tags "netscape"
				description "Web interface for online shop interaction"
				technology "Web Browser"
			}

			user_database = container "User database" {
				tags "database"
				description "Stores information about registered cusomers and their cart contents"
				technology "PostgreSQL"
			}

			user_service = container "User service" {
				description "Handles customer information query and authentication"
				technology "C++"

				-> user_database "Reads or updates customer information" "SQL"
			}

			cart_service = container "Cart service" {
				description "Handles customer cart management"
				technology "C++"

				-> user_database "Reads and updates cart information" "SQL"
			}

			product_database = container "Products database" {
				tags "database"
				description "Stores information about available shop goods"
				technology "PostgreSQL???"
			}

			shop_service = container "Shop service" {
				description "Handles shop item queries"
				technology "C++"

				-> product_database "Queries and update information about available shop goods" "SQL"
			}

			order_service = container "Order service" {
				description "Handles order processing"
				technology "C++"

				-> cart_service "Makes API call to receive cart contents to order to"
				-> shop_service "Make API call to check cart contents availability to"
				-> payment_system "Makes API call to forward payment handling to" "JSON/HTTP"
				-> warehouse_system "Makes API call to query and update information about ordered goods to" "JSON/HTTP"
				-> notification_system "Makes API call to forward notification to a respective customer after successful order to" "JSON/HTTP"
			}

			webapp_backend = container "Web application backend" {
				description "Backend service for handling shop functionality via JSON/HTTPS API"
				technology "C++"

				-> user_service "Makes API calls to read and update customer information and handle authentication"
				-> cart_service "Makes API calls to read and update customer's cart contents"
				-> shop_service "Makes API calls to search, view and update (admin only) available shop goods"
				-> order_service "Makes API calls to make orders"
			}

			guest -> client_webapp "Seeks and views store goods, can register an account"
			customer -> client_webapp "Can seek, view, maintain a cart (view, add, remove items) and order goods from the cart; makes payment"
			admin -> client_webapp "Adds, removes, updates store items; may manage registered users"

			client_webapp -> webapp_backend "Makes API calls to" "JSON/HTTPS"
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

			guest -> online_shop.client_webapp "Guest visits online shop website, visit register page and fills in required info"
			online_shop.client_webapp -> online_shop.webapp_backend "Issues a registration request to"
			online_shop.webapp_backend -> online_shop.user_service "Forwards registration request with data"
			online_shop.user_service -> online_shop.user_database "Inserts a new customer entry into"
		}

		dynamic online_shop "user-info-query" {
			autoLayout lr
			description "User information query process"

			admin -> online_shop.client_webapp "Admin visits online shop website, authenticates, visit admin page and searches for a login"
			online_shop.client_webapp -> online_shop.webapp_backend "Issues a search request to"
			online_shop.webapp_backend -> online_shop.user_service "Forwards search by login request to"
			online_shop.user_service -> online_shop.user_database "Makes a query by login to"
		}

		dynamic online_shop "product-creation" {
			autoLayout lr
			description "Product creation process"

			admin -> online_shop.client_webapp "Admin visits online shop website, authenticates, visit admin page and adds a new item info"
			online_shop.client_webapp -> online_shop.webapp_backend "Issues a new product creation request to"
			online_shop.webapp_backend -> online_shop.shop_service "Forwards product creation request to"
			online_shop.shop_service -> online_shop.product_database "Insert a new product entry into"
		}

		dynamic online_shop "product-listing" {
			autoLayout lr
			description "Product listing process"

			{ /* Parallel sequences' syntax sucks */
				{
					guest -> online_shop.client_webapp "Guest visits online shop website"
				}
				{
					customer -> online_shop.client_webapp "Customer visits online shop website"
				}
			}
			online_shop.client_webapp -> online_shop.webapp_backend "Issues a product listing request to"
			online_shop.webapp_backend -> online_shop.shop_service "Forwards product listing request to"
			online_shop.shop_service -> online_shop.product_database "Makes a product query from"
		}

		dynamic online_shop "add-product-to-cart" {
			autoLayout lr
			description "Adding a product to cart process"

			customer -> online_shop.client_webapp "Customer visits online shop website then clicks on a specific button"
			online_shop.client_webapp -> online_shop.webapp_backend "Issues a product cart insertion request to"
			online_shop.webapp_backend -> online_shop.cart_service "Forwards product cart insertion request to"
			online_shop.cart_service -> online_shop.user_database "Inserts new cart entry into"
		}

		dynamic online_shop "list-cart" {
			autoLayout lr
			description "Listing a cart contents process"

			customer -> online_shop.client_webapp "Customer visits online shop website then visits cart page"
			online_shop.client_webapp -> online_shop.webapp_backend "Issues a cart listing request to"
			online_shop.webapp_backend -> online_shop.cart_service "Forwards cart listing request to"
			online_shop.cart_service -> online_shop.user_database "Queries user's cart entries from"
		}

		styles {
			element external {
				background #aaaaaa
			}

			element database {
				shape cylinder
			}

			element netscape {
				shape webbrowser
			}
		}
	}

	configuration {
		scope softwaresystem
	}
}
