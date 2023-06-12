idf_component_register(SRCS "main.c"
                            "CAN/user_can.c"
                            "HY_SRF/hy_srf.c"
                            "App/app.c"
                            "IR/ir.c"
                        
                    INCLUDE_DIRS "."
                                 "CAN/"
                                 "HY_SRF/"
                                 "APP/"
                                 "IR/")
