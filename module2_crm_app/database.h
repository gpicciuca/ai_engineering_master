#ifndef __DATABASE_H__
#define __DATABASE_H__

#include <cstdint>
#include <fstream>
#include <map>
#include <memory>
#include <string>

#include "customers.h"

/// @brief Manages all input and output with the actual data store
class Database {
 public:
  // No default, move and copy constructors/operators
  Database() = delete;
  Database(const Database &) = delete;
  Database &operator=(const Database &) = delete;
  Database(Database &&) = delete;
  Database &operator=(Database &&) = delete;

  explicit Database(const std::string &database_path);
  ~Database();

  /// @brief Adds a new customer to the database
  /// @param name Name
  /// @param surname Surname
  /// @return ID assigned to the newly added customer
  Customer::ID AddCustomer(const std::string &name, const std::string &surname);

  /// @brief Checks if a customer already exists with the given name and
  /// surname. Both values are required.
  /// @param name Name to search for
  /// @param surname Surname to search for
  /// @return True if found, false otherwise.
  bool HasCustomer(const std::string &name, const std::string &surname) const;

  /// @brief Checks if a Customer with the specified ID exists
  /// @param customer_id Customer ID
  /// @return True if found, false otherwise.
  bool HasCustomer(Customer::ID customer_id) const;

  /// @brief Returns a read-only reference to a Customer object. Must be called
  /// after checking if the Customer exists! Attempting to retrieve a
  /// non-existed Customer will lead to a segmentation fault.
  /// Use HasCustomer() to check if the entry exists.
  /// @param customer_id ID of customer to fetch
  /// @return Reference to customer object
  const Customer &GetCustomer(const Customer::ID customer_id) const;

  /// @brief Updates the information of a customer
  /// @param id Customer ID
  /// @param name New name
  /// @param surname New surname
  /// @return False if customer was does not exist, true otherwise.
  bool UpdateClientInfo(const Customer::ID id, const std::string &name,
                        const std::string &surname);

  /// @brief Removes a customer from the database
  /// @param id Customer ID to remove
  /// @return True on success, false if not found.
  bool RemoveCustomer(const Customer::ID id);

  /// @brief Adds a new interaction to the specified customer
  /// @param id Customer ID
  /// @param when String containing a valid date
  /// @param what Description of the interaction
  /// @return True on success, false if customer not found.
  bool AddInteraction(const Customer::ID id, const std::string &when,
                      const std::string &what);

  /// @brief Collections all interactions of a customer that happened within a
  /// specified time interval.
  /// @param id Customer ID
  /// @param from_timestamp Start date as a UNIX Timestamp
  /// @param to_timestamp End date as a UNIX Timestamp
  /// @param interactions Vector where all found interactions shall be pushed
  /// into
  void GetCustomerInteractionsInRange(
      const Customer::ID id, const std::time_t from_timestamp,
      const std::time_t to_timestamp,
      std::vector<std::shared_ptr<Interaction>> &interactions) const;

  /// @brief Allows direct read-only access to all customers
  /// @return Reference to customers
  const std::map<Customer::ID, Customer> &GetCustomers() const;

 private:
  /// @brief Loads an existing database file into memory
  /// @return True if file could be opened, false otherwise.
  bool LoadFromFile();

  /// @brief Writes all customers to the database file.
  void SaveDatabase() const;

  /// @brief Finds the currently highest assigned ID among existing Customers.
  /// Used in conjunction with AddCustomer to assign an ID to new customers.
  /// @return Current highest ID, or 1 by default
  Customer::ID GetHighestCustomerID() const;

  /// @brief Path where database is loaded from/saved to
  std::string database_path_;

  /// @brief Keeps all customers in memory
  std::map<Customer::ID, Customer> customers_;
};

#endif  // __DATABASE_H__
