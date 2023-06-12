#ifndef FACES_DATABASEENTRY_HPP
#define FACES_DATABASEENTRY_HPP

#include <utils/LookableAttributes.hpp>

namespace faces {

    /**
     * A base class for all of the database entries
     *
     * @tparam DerivedT - a type of the class which inherited from it
     */
    template<typename DerivedT>
    class DatabaseEntry : public LookableFields<DerivedT> {
    public:
        /**
         * Initializes attributes with the given map
         *
         * @param attributes - a map {attribute name: value}
         *
         * @note it continues initializing attributes even after a error
         *
         * @return were all of the given attributes(from parameter) initialized
         */
        bool initAttributes(std::map<std::string, std::any> const &attributes) {
            bool ok = true;
            for (auto const &[name, value] : attributes) {
                try {
                    LookableFields<DerivedT>::set(name, value);
                } catch (std::exception const &e) {
                    spdlog::warn("Error during initialization of database entry`s fields: {}", e.what());
                    ok = false;
                }
            }
            return ok;
        }

        /**
         * Serializes attributes
         *
         * @return a map {attribute name: value}, which is valid to pass to the 'initAttributes' method
         */
        [[nodiscard]] std::map<std::string, std::any> getAttributes() const {
            std::map<std::string, std::any> res;

            for (std::string const &attribute : this->_attributeNames) {
                res[attribute] = LookableFields<DerivedT>::get(attribute);
            }

            return res;
        }

    protected:
        /**
         * @param derived - a reference to the inherited class (ie. '*this')
         */
        explicit DatabaseEntry(DerivedT &derived) : LookableFields<DerivedT>(derived) {}

        using LookableFields<DerivedT>::_registerField;

    };

}

#endif //FACES_DATABASEENTRY_HPP
