

idf_component_register(SRCS 
                            "app_main.c"
                            "src/hal/ds_timer.c"
                            "src/hal/ds_nvs.c"
                            "src/ds_system_data.c"

                            "src/fs/ds_spiffs.c"
                    
                       INCLUDE_DIRS 
                            "."
                            "./include"
                            "./include/hal"
                            "./include/fs")
