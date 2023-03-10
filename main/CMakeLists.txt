

idf_component_register(SRCS 
                            "app_main.c"

                            "src/ds_system_data.c"

                            "src/hal/ds_timer.c"
                            "src/hal/ds_nvs.c"
                            "src/hal/ds_gpio.c"
                            "src/hal/ds_i2c.c"
                            "src/hal/ds_spi.c"

                            "src/fs/ds_spiffs.c"

                            "src/driver/ds_ft6336.c"
                            "src/driver/ds_screen.c"

                            "src/data/ds_data_page.c"
                            "src/net/ds_wifi_ap.c"
                            "src/net/ds_wifi_sta.c"
                            "src/net/ds_http_server.c"
                       INCLUDE_DIRS 
                            "."
                            "./include"
                            "./include/hal"
                            "./include/fs"
                            "./include/driver"
                            "./include/data"
                            "./include/net"
                        EMBED_FILES 
                            "./html/favicon.ico"
                            "./html/setting.html"
                            )
