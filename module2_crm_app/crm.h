#ifndef __CRM_H__
#define __CRM_H__

#include <ctime>
#include <string>

#include "database.h"

/// @brief Manages all client information and interfaces directly with the
/// database
class CRM {
 public:
  // No default, move and copy constructors/operators
  CRM() = delete;
  CRM(const CRM&) = delete;
  CRM& operator=(const CRM&) = delete;
  CRM(CRM&&) = delete;
  CRM& operator=(CRM&&) = delete;

  explicit CRM(const std::string& database_path);

  /// @brief Adds a new customer
  /// @param name Name of customer
  /// @param surname Surname of customer
  /// @return True if added, False if already exists
  bool AddCustomer(const std::string& name, const std::string& surname);

  /// @brief Prints all customer information to terminal
  /// @return True if database has customers, false if none available
  bool PrintAllCustomers() const;

  /// @brief Prints the client information to terminal
  /// @param customer_ids Set of client IDs whose information shall be printed
  void PrintCustomersByID(const std::vector<Customer::ID>& customer_ids) const;

  /// @brief Fetches the Client IDs of all customers that match the given search
  /// criterias. All arguments are optional.
  /// @param id Client ID
  /// @param name Name
  /// @param surname Surname
  /// @param found_customers Where to store all found Client IDs
  /// @return True if clients were found, false otherwise
  bool FindCustomers(const std::string& id, const std::string& name,
                     const std::string& surname,
                     std::vector<Customer::ID>& found_customers) const;

  /// @brief Gets the customer under the specified ID
  /// @param id Client ID
  /// @return Read-only Customer object containing all its information
  const Customer& GetCustomer(const Customer::ID id) const;

  /// @brief Updates the information of a client
  /// @param id Client ID
  /// @param name New name
  /// @param surname New surname
  /// @return False if client was not found, true otherwise
  bool UpdateClientInfo(const Customer::ID id, const std::string& name,
                        const std::string& surname);

  /// @brief Deletes a client from the database
  /// @param id Client ID
  /// @return False if client was not found, true otherwise
  bool RemoveCustomer(const Customer::ID id);

  /// @brief Adds a new interaction to a given client
  /// @param id Client ID
  /// @param when String containing a valid date
  /// @param what Description of the interaction
  /// @return False if client was not found, true otherwise
  bool AddInteraction(const Customer::ID id, const std::string& when,
                      const std::string& what);

  /// @brief Prints all client interactions in a user-specified time interval
  /// @param id Client ID
  /// @param from_timestamp Start date as a UNIX Timestamp
  /// @param to_timestamp End date as a UNIX Timestamp
  /// @return True if client had any interactions in the given timeframe, false
  /// otherwise
  bool PrintCustomerInteractions(const Customer::ID id,
                                 const std::time_t from_timestamp,
                                 const std::time_t to_timestamp) const;

 private:
  Database database_;
};

#endif  // __CRM_H__
