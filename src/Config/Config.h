#ifndef FACES_CONFIG_H
#define FACES_CONFIG_H


#include <spdlog/spdlog.h>

#include <miniconf.h>

#include <utils/utils.h>


namespace faces {

/**
 * Adds a new option to the config config
 *
 * @param name              - name of the option
 * @param shortFlag         - name of the command line flag (e.g. --test)
 * @param defaultVal        - value, the option will have before the initialization from command line or file
 * @param valueRequired     - flag which indicates whether the option should be initialized or not
 * @param valueDescription  - human-readable description of the option
 */
#define FACES_ADD_CONFIG_OPTION(name, shortFlag, defaultVal, valueRequired, valueDescription) \
        Config::getInstance().config.option(name).shortflag(shortFlag).defaultValue(defaultVal) \
                                        .required(valueRequired).description(valueDescription);

/**
 * Adds new options to the config outside of a function
 * It creates an inline function and assigns its return value to a global variable and thus calling it
 *
 * @param name  - unique postfix for function and variable
 * @param ...   - list of @ref FACES_ADD_CONFIG_OPTION macro calls; basically, it is a function body
 */
#define FACES_AUGMENT_CONFIG(name, ...) \
        inline static int __config_augment_dummy_fn_ ## name() { \
            __VA_ARGS__ \
            return 0; \
        } \
        inline static int __config_augment_dummy_var_ ## name = __config_augment_dummy_fn_ ## name();


    /**
     * Singleton wrapper around the config manager
     */
    class Config {
    public:
        /// the actual config manager
        miniconf::Config config = miniconf::Config();

        /**
         * @return a static instance of this class, creating it of necessary
         */
        static Config &getInstance() {
            static Config instance;
            return instance;
        }

        /**
         * Accesses the configuration value
         * A proxy operator to simplify accessing to the actual config`s operator[]
         *
         * @param flag - name of the option
         *
         * @return a reference to the value of the option
         *         OR an empty miniconf::Value object if the option with the given name does not exist
         */
        miniconf::Value &operator[](const std::string &flag) {
            return config[flag];
        }

        /**
         * Accesses the configuration value
         * A proxy operator to simplify accessing to the actual config`s operator[]
         *
         * @param flag - name of the option
         *
         * @throws std::out_of_range - if the option with the given name does not exist
         *
         * @return a value of the option
         */
        miniconf::Value const &operator[](const std::string &flag) const {
            return config[flag];
        }

        /**
         * A helper method to get a path to some data file
         *
         * @param name          - a name of the config option with the name of the file
         * @param dataSubdir    - a subdirectory of the 'dataDirectory' where the data file is located
         *
         * @return a path to the required file
         *         OR an empty string if either 'dataDirectory' or the option
         *         with the given name does not exist
         */
        [[nodiscard]] std::string getDataPath(std::string const &name,
                                              std::string const &dataSubdir = "") const {
            try {
                return config["dataDirectory"].getString() + dataSubdir + "/" + config[name].getString();
            } catch (std::out_of_range &e) {
                spdlog::error("Cannot get a data entry with the name '{}' from '{}' in the config!",
                              name, dataSubdir);
                return "";
            }
        }

        /**
         * @returns a path to a model file
         * @see getDataPath
         */
        [[nodiscard]] std::string getModelPath(std::string const &name,
                                               std::string const &dataSeparator = "") const {
            return getDataPath(name, "/models" + dataSeparator);
        }


        Config(Config const &) = delete;

        void operator=(Config const &) = delete;

    protected:
        Config() = default;

    };

    FACES_AUGMENT_CONFIG(base,
                         FACES_ADD_CONFIG_OPTION("dataDirectory", "data", FACES_ROOT_DIRECTORY "/data",
                                 false, "A path to a directory containing all the data")
    )

}

#endif //FACES_CONFIG_H
