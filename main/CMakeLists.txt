

idf_component_register(SRCS 
                            "app_main.c"

                            "src/ds_system_data.c"

                            "src/hal/ds_timer.c"
                            "src/hal/ds_nvs.c"
                            "src/hal/ds_gpio.c"
                            "src/hal/ds_i2c.c"

                            "src/fs/ds_spiffs.c"

                            "src/driver/ds_ft6336.c"
                    
                       INCLUDE_DIRS 
                            "."
                            "./include"
                            "./include/hal"
                            "./include/fs"
                            "./include/driver")
