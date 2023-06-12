#ifndef FACES_DATABASE_HPP
#define FACES_DATABASE_HPP

#include "DatabaseEntry.hpp"

namespace faces {

    /**
     * A base class for all of the face databases
     *
     * @tparam EntryT       - type of the stored information, should be derived from faces::DatabaseEntry
     * @tparam IdentifierT  - type of the identifier of database entries (key)
     */
    template<typename EntryT, typename IdentifierT>
    class Database {
    public:
        static_assert(std::is_constructible<EntryT, std::map<std::string, std::any>>::value,
                      "EntryT of the Database should be a subclass of faces::DatabaseEntry, "
                      "and provide access to its protected constructor");

        /**
         * @param id - identifier of the entry
         *
         * @throws std::out_of_range - if an entry with the given id is not found
         *
         * @return an entry with the given id
         */
        virtual EntryT get(IdentifierT id) = 0;

        /**
         * @return a vector with ids from database
         */
        virtual std::vector<IdentifierT> getEntriesList() = 0;

        /**
         * Adds a new entry to the database
         *
         * @param entry - an entry to add
         *
         * @return id of the added entry
         */
        virtual IdentifierT add(EntryT const &entry) = 0;

        /**
         * Updates the entry
         *
         * @param id    - id of the entry
         * @param entry - a new value
         *
         * @throws std::out_of_range - if an entry with the given id is not found
         */
        virtual void update(IdentifierT id, EntryT const &entry) = 0;

        /**
         * Saves a database
         *
         * @return a successfulness of the operation
         */
        virtual bool save() = 0;

        /**
         * Loads a database
         *
         * @return a successfulness of the operation
         */
        virtual bool load() = 0;

    };

}

#endif //FACES_DATABASE_HPP
