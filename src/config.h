#ifndef JACKALPUP_CONFIG_H
#define JACKALPUP_CONFIG_H



#define APP_VERSION_MAJOR 0
#define APP_VERSION_MINOR 1
#define APP_VERSION_PATCH 0

#define APP_VERSION \
    VK_MAKE_VERSION(APP_VERSION_MAJOR, APP_VERSION_MINOR, APP_VERSION_PATCH)



//Config name
#ifdef TESTBED
#define APP_NAME "Testbed"
#else
#define APP_NAME "JackalPup"
#endif



#define ENGINE_NAME "Jackal"




#endif