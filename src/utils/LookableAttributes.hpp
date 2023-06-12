#ifndef FACES_LOOKABLEATTRIBUTES_HPP
#define FACES_LOOKABLEATTRIBUTES_HPP

#include <string>
#include <sstream>
#include <utility>
#include <any>

#include <semimap.h>

/**
 * Register a class field, so it can be accessed by its name
 *
 * @note this macro should only be called in class inherited from faces::LookableAttributes
 * @note this macro is intended to be used in classes inherited from faces::DatabaseEntry
 *       to register its attributes
 *
 * @param cls - a name of the class, this attribute from
 * @parm attribute - a name of the registered attribute
 */
#define FACES_REGISTER_ACCESSOR(cls, attribute) \
    int __faces_attribute_accessor_dymmy_ ## attribute = _registerField(#attribute, &cls::attribute);

namespace faces {

    /**
     * @tparam T - a type, get name of which
     *
     * @return a string name of the given template type
     */
    template<typename T>
    std::string getTypeStr() {
        std::string typeStr = __PRETTY_FUNCTION__;
        std::size_t start = typeStr.find("[with T = ") + 10;
        std::size_t end = typeStr.find(';');

        return typeStr.substr(start, end - start);
    }

/**
 * Checks if type stored in std::any is equal to the tried one, and if so,
 * casts std::any to that type and then casts it to the desired type
 *
 * @note this macro would be undefed soon
 *
 * @param dstType   - a name of the desired type, we want std::any to cast to
 * @param triedType - a tried type of std::any
 */
#define _FACES_TRY_CAST_ANY(dstType, triedType) \
            if(value.type() == typeid(triedType)) { \
                auto realValue = std::any_cast<triedType>(value); \
                return static_cast<dstType>(realValue); \
            }

/**
 * Creates a template specialization of the 'anyCast' function,
 * which brute-forces type of the passed std::any value and converts value to it,
 * and then to the requested type
 *
 * @see _FACES_TRY_CAST_ANY
 *
 * @note this macro would be undefed soon
 *
 * @parm forType - a type, create a specialization for
 */
#define _FACES_ADD_ANY_CAST_FOR_NUM(forType) \
            template <> \
            forType anyCast(std::any const &value) { \
                _FACES_TRY_CAST_ANY(forType, int) \
                _FACES_TRY_CAST_ANY(forType, unsigned int) \
                _FACES_TRY_CAST_ANY(forType, float) \
                _FACES_TRY_CAST_ANY(forType, double) \
                _FACES_TRY_CAST_ANY(forType, short) \
                _FACES_TRY_CAST_ANY(forType, unsigned short) \
                _FACES_TRY_CAST_ANY(forType, std::size_t) \
                return std::any_cast<forType>(value); \
            }

    /**
     * An unstrict version of std::any_cast,
     * which can cast std::any to the requested type even if its type do not match the requested one
     *
     * @note if you want to make this function support more types, you should add a specialization bellow
     *
     * @tparam T    - a requested type, convert std::any to
     * @param value - value, we are casting
     */
    template<typename T>
    T anyCast(std::any const &value) {
        return std::any_cast<T>(value);
    }

    // Specializations of the anyCast function

    _FACES_ADD_ANY_CAST_FOR_NUM(int)

    _FACES_ADD_ANY_CAST_FOR_NUM(unsigned int)

    _FACES_ADD_ANY_CAST_FOR_NUM(double)

    _FACES_ADD_ANY_CAST_FOR_NUM(float)

    _FACES_ADD_ANY_CAST_FOR_NUM(short)

    _FACES_ADD_ANY_CAST_FOR_NUM(unsigned short)

    _FACES_ADD_ANY_CAST_FOR_NUM(std::size_t)

#undef _FACES_TRY_CAST_ANY
#undef _FACES_ADD_ANY_CAST_FOR_NUM

    /**
     * A base class of the class attribute accessor,
     * used to read and modify a field of any class in an unified way
     *
     * @tparam BaseClass - a class, where attribute is
     */
    template<typename BaseClass>
    class AttributeAccessor {
    public:
        /// A name of the attribute
        const std::string key;

        explicit AttributeAccessor(std::string aKey) : key(std::move(aKey)) {}

        /**
         * Sets the value of the p's attribute to 'value'
         */
        virtual void set(BaseClass &p, std::any const &value) const = 0;

        /**
         * Gets a value of the p's attribute
         */
        virtual std::any get(BaseClass &base) const = 0;
    };

    /**
     * It implements attribute value manipulation,
     * but it needs to know attribute's type, so we need to use AttributeAccessor base to work with it
     *
     * @tparam BaseClass - a class, where attribute is
     * @tparam T         - type of the attribute
     */
    template<typename BaseClass, typename T>
    class AttributeAccessorImpl : public AttributeAccessor<BaseClass> {
    public:
        typedef T BaseClass::*TMember;

        /// A pointer to BaseClass attribute, we are working with
        TMember member;

        /**
         * @param aKey - name of the attribute
         * @param t - a pointer to the attribute (eg. &Cls::attr)
         */
        AttributeAccessorImpl(const std::string &aKey, const TMember t)
                : AttributeAccessor<BaseClass>(aKey), member(t) {}

        void set(BaseClass &p, std::any const &value) const override {
            (p.*member) = anyCast<T>(value);
        }

        std::any get(BaseClass &base) const override {
            return base.*member;
        }
    };

    /**
     * A base class for classes with attributes accessible by name
     *
     * @tparam DerivedT - a name of the class, which has inherited from LookableFields
     */
    template<typename DerivedT>
    class LookableFields {
    public:
        /**
         * Sets a value to the attribute with the given name
         *
         * @param name - name of the attribute
         * @param value - value of the attribute
         *
         * @note you should call this function via a parent class (eg. LookableFields<DerivedT>::set)
         *
         * @throws std::out_of_range - if an attribute with the requested name is not found
         */
        void set(std::string const &name, std::any const &value) {
            if (!_setters::contains(name)) {
                std::stringstream ss;
                ss << "Attribute '" << name << "' of the class '" << getTypeStr<DerivedT>()
                   << "' is not found";
                throw std::out_of_range(ss.str());
            }

            _setters::get(name)->set(_derived, value);
        }

        /**
         * Gets a value of the attribute
         *
         * @note you should call this function via a parent class (eg. LookableFields<DerivedT>::get)
         *
         * @return a value of the requested attribute
         *         OR an empty std::any, in case if an attribute with the requested name is not found
         */
        [[nodiscard]] std::any get(std::string const &name) const {
            if (!_setters::contains(name)) {
                return std::any();
            }
            return _setters::get(name)->get(_derived);
        }

    protected:
        struct __Tag {
        };

        /// A map {attribute name: accessor class}
        using _setters = semi::static_map<std::string, AttributeAccessor<DerivedT> *, __Tag>;
        /// A vector of the registered attribute names (it is necessary coz semi::static_map hides a real map)
        static std::vector<std::string> _attributeNames;

        /**
         * @param derived - a reference to the inherited class (ie. '*this')
         */
        explicit LookableFields(DerivedT &derived) : _derived(derived) {}

        /**
         * Registers an accessor for attribute
         *
         * @tparam FieldT - type of the attribute
         * @param name - name of the attribute
         * @param field - a pointer to the attribute
         *
         * @note you probably should use a FACES_REGISTER_ACCESSOR macro
         *
         * @return - a dummy value (it is necessary to assign this function's return value to a variable,
         *                          and call it outside of any functions)
         */
        template<typename FieldT>
        int _registerField(std::string const &name, FieldT DerivedT::*field) {
            _setters::get(name) = new AttributeAccessorImpl<DerivedT, FieldT>(name, field);
            _attributeNames.emplace_back(name);
            return 0;
        }

    private:
        /// A reference to the class, where attributes are located
        DerivedT &_derived;

    };

    // initializing a static attribute
    template<typename DerivedT>
    std::vector<std::string> LookableFields<DerivedT>::_attributeNames = {};

}

#endif //FACES_LOOKABLEATTRIBUTES_HPP
