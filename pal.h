#ifndef PAL_H
#define PAL_H

#include <string>

namespace dps {
    using namespace std;
    /**
     * @brief The PAL class Platform Abstraction Layer. Implements platform-dependent functions
     */
    class PAL
    {
    public:
        static string   path_get_data_dir();
        static string   path_get_config_dir();
    };
}
#endif // PAL_H
