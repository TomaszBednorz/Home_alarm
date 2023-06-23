#include "user_can.h"
#include "hy_srf.h"
#include "ir.h"
#include "app.h"

void app_main()
{
    UserCan_Init();
    HySrf_Init();
    Ir_Init();
    App_Init();
}

