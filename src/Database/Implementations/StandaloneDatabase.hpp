#ifndef FACES_STANDALONEDATABASE_HPP
#define FACES_STANDALONEDATABASE_HPP

#include <utility>
#include <fstream>

#include <picojson.h> // from third-party/miniconf

#include <Database/Database.hpp>

namespace faces {

    /**
     * A standalone implementation of the Database, which uses a JSON file as a storage
     */
    template<typename EntryT, typename IdentifierT = unsigned long>
    class StandaloneDatabase : public Database<EntryT, IdentifierT> {
    public:
        /**
         * @param path - path to the JSON file of the DB
         */
        explicit StandaloneDatabase(std::string path) : _path(std::move(path)) {}

        EntryT get(IdentifierT id) override;

        std::vector<IdentifierT> getEntriesList() override;

        IdentifierT add(EntryT const &entry) override;

        void update(IdentifierT id, EntryT const &entry) override;

        bool save() override;

        bool load() override;

    protected:
        const std::string _path;

        picojson::value _data;

        /**
         * Parses a JSON from the given file stream
         *
         * @param file - a stream to parse from
         *
         * @throws std::logic_error - if the parsing failed
         */
        void _loadJson(std::ifstream &file);

        /**
         * Gets a json object of the entry with the given id
         *
         * @param id - an id of the requested entry
         *
         * @throws std::out_of_range - if the requested id is not found
         * @throws std::logic_error  - if tge object with the requested id has incorrect type
         *
         * @return a reference to the json object at the requested id
         */
        picojson::object &_getEntryJson(IdentifierT const &id);

        /**
         * Converts the actual value of the json value container to std::any
         *
         * @throws std::bad_cast - if the given json value has an unsupported type
         */
        static std::any _valueToAny(picojson::value const &value);

        /**
         * Converts a given std::any value to the json value container
         *
         * @throws std::bad_cast - if the given std::any value has an unsupported type
         */
        static picojson::value _anyToValue(std::any const &value);

        /**
         * Converts id to string
         */
        static std::string _getStringId(IdentifierT const &id);

        /**
         * Initializes a json object with entry's values
         */
        static picojson::object _entryToJson(EntryT const &entry);

    };

    template<typename EntryT, typename IdentifierT>
    EntryT StandaloneDatabase<EntryT, IdentifierT>::get(IdentifierT id) {
        picojson::object const &entryObj = _getEntryJson(id);

        std::map<std::string, std::any> entryAttributes;
        for (auto const &[name, valueJson] : entryObj) {
            std::any value = _valueToAny(valueJson);
            entryAttributes[name] = value;
        }

        return EntryT(entryAttributes);
    }

    template<typename EntryT, typename IdentifierT>
    std::vector<IdentifierT> StandaloneDatabase<EntryT, IdentifierT>::getEntriesList() {
        std::vector<IdentifierT> res;

        auto &obj = _data.get<picojson::object>();
        for (auto const &[key, value] : obj) {
            if constexpr (!std::is_same_v<picojson::object::key_type, IdentifierT>) {
                if constexpr (std::is_arithmetic_v<IdentifierT>) {
                    std::istringstream iss(key);
                    IdentifierT id;
                    iss >> id;
                    res.emplace_back(id);
                } else {
                    res.emplace_back(static_cast<IdentifierT>(key));
                }
            } else {
                res.emplace_back(key);
            }
        }

        return res;
    }

    template<typename EntryT, typename IdentifierT>
    IdentifierT StandaloneDatabase<EntryT, IdentifierT>::add(const EntryT &entry) {
        std::vector<IdentifierT> entries = getEntriesList();
        IdentifierT nextId = *std::max_element(entries.begin(), entries.end()) + 1;

        auto &obj = _data.get<picojson::object>();

        picojson::value entryJsonValue(_entryToJson(entry));
        obj[_getStringId(nextId)] = entryJsonValue;

        return nextId;
    }

    template<typename EntryT, typename IdentifierT>
    void StandaloneDatabase<EntryT, IdentifierT>::update(IdentifierT id, const EntryT &entry) {
        picojson::object &entryObj = _getEntryJson(id);
        entryObj = _entryToJson(entry);
    }

    template<typename EntryT, typename IdentifierT>
    bool StandaloneDatabase<EntryT, IdentifierT>::save() {
        try {
            std::ofstream file(_path);
            file.exceptions(file.exceptions() | std::ifstream::failbit | std::ifstream::badbit);

            if (!_data.is<picojson::object>()) {
                _data = picojson::value(picojson::object());
            }

            _data.serialize(std::ostream_iterator<char>(file), true);
        } catch (std::exception &e) {
            spdlog::error("Cannot save a database to '{}': {}", _path, e.what());
            return false;
        }

        return true;
    }

    template<typename EntryT, typename IdentifierT>
    bool StandaloneDatabase<EntryT, IdentifierT>::load() {
        try {
            std::ifstream file(_path);
            if (!file.is_open() || file.bad()) {
                throw std::ios_base::failure(strerror(errno));
            }

            _loadJson(file);
        } catch (std::exception &e) {
            spdlog::error("Cannot load a database from '{}': {}", _path, e.what());
            return false;
        }

        return true;
    }

    template<typename EntryT, typename IdentifierT>
    void StandaloneDatabase<EntryT, IdentifierT>::_loadJson(std::ifstream &file) {
        std::string err = picojson::parse(_data, file);
        if (!err.empty()) {
            throw std::logic_error(err);
        }
        if (!_data.is<picojson::object>()) {
            throw std::logic_error("json file has an incorrect format");
        }
    }

    template<typename EntryT, typename IdentifierT>
    picojson::object &StandaloneDatabase<EntryT, IdentifierT>::_getEntryJson(const IdentifierT &id) {
        picojson::object &obj = _data.get<picojson::object>();

        std::string strId = _getStringId(id);

        if (obj.find(strId) == obj.end()) {
            throw std::out_of_range("Entry with ID='" + strId + "' does not exist in the database");
        }

        picojson::value &entryJson = obj[strId];
        if (!entryJson.is<picojson::object>()) {
            throw std::logic_error("Entry with ID='" + strId + "' has an incorrect format");
        }
        return entryJson.get<picojson::object>();
    }

    template<typename EntryT, typename IdentifierT>
    std::any StandaloneDatabase<EntryT, IdentifierT>::_valueToAny(const picojson::value &value) {
        if (value.is<bool>()) {
            return value.get<bool>();
        }
        if (value.is<double>()) {
            return value.get<double>();
        }
        if (value.is<std::string>()) {
            return value.get<std::string>();
        }
        if (value.is<picojson::object>()) {
            return value.get<picojson::object>();
        }
        throw std::bad_cast();
    }

    template<typename EntryT, typename IdentifierT>
    picojson::value StandaloneDatabase<EntryT, IdentifierT>::_anyToValue(const std::any &value) {
        if (value.type() == typeid(bool)) {
            return picojson::value(std::any_cast<bool>(value));
        }
        if (value.type() == typeid(std::string)) {
            return picojson::value(std::any_cast<std::string>(value));
        }
        return picojson::value(anyCast<double>(value));
    }

    template<typename EntryT, typename IdentifierT>
    std::string StandaloneDatabase<EntryT, IdentifierT>::_getStringId(const IdentifierT &id) {
        std::stringstream ss;
        ss << id;
        return ss.str();
    }

    template<typename EntryT, typename IdentifierT>
    picojson::object StandaloneDatabase<EntryT, IdentifierT>::_entryToJson(const EntryT &entry) {
        picojson::object res;

        const std::map<std::string, std::any> attributes = entry.getAttributes();
        for (auto const &[key, value] : attributes) {
            res[key] = _anyToValue(value);
        }

        return res;
    }

}

#endif //FACES_STANDALONEDATABASE_HPP
