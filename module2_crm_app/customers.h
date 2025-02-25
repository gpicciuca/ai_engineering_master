#ifndef __CUSTOMERS_H__
#define __CUSTOMERS_H__

#include <ctime>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "utilities.h"

#define DATE_FORMAT "%d/%m/%Y %H:%M"
#define SERIALIZATION_DELIMITER '\t'
#define INVALID_CUSTOMER_ID 0U

/// @brief Holds the information for a single interaction
struct Interaction {
  std::string when_;
  std::string what_;

  Interaction() = default;

  explicit Interaction(const std::string& when, const std::string& what)
      : when_{when}, what_{what} {}

  /// @brief Stream overload to easily serialize the data into a stream.
  /// Defined as a friend-method here for convenience, instead of having it in
  /// global-scope.
  /// @param os Output stream where to serialize the data into
  /// @param interaction Interaction object to serialue
  /// @return Reference to the output stream
  friend std::ostream& operator<<(std::ostream& os,
                                  const Interaction& interaction) {
    os << interaction.when_ << SERIALIZATION_DELIMITER;
    os << interaction.what_;
    return os;
  }

  /// @brief Checks if the current interaction has a date within the supplied
  /// timeframe
  /// @param from_timestamp Start date as a UNIX Timestamp
  /// @param to_timestamp End date as a UNIX Timestamp
  /// @return True if date is within the given range, false otherwise
  bool InRange(const std::time_t from_timestamp,
               const std::time_t to_timestamp) const {
    std::time_t timestamp{};
    utilities::to_timestamp(when_, DATE_FORMAT, timestamp);

    return (timestamp >= from_timestamp && timestamp <= to_timestamp);
  }

  /// @brief Convenience method to print the information of this interaction to
  /// screen
  void Print() const { std::cout << when_ << "\t\t" << what_ << std::endl; }
};

/// @brief Holds all the information of a Customer
struct Customer {
  /// @brief Type of the Customer ID
  using ID = std::uint32_t;

  /// @brief Associated ID for this customer
  ID id_;
  /// @brief Customer Name
  std::string name_;
  /// @brief Customer Surname
  std::string surname_;
  /// @brief Interactions with this Customer
  std::vector<std::shared_ptr<Interaction>> customer_interactions_;

  Customer() = default;

  explicit Customer(ID id, const std::string& name, const std::string surname)
      : id_{id}, name_{name}, surname_{surname}, customer_interactions_{} {}

  explicit Customer(const std::string& name, const std::string surname)
      : id_{}, name_{name}, surname_{surname}, customer_interactions_{} {}

  explicit Customer(Customer&& customer)
      : id_{customer.id_},
        name_{std::move(customer.name_)},
        surname_{std::move(customer.surname_)},
        customer_interactions_{std::move(customer.customer_interactions_)} {}

  Customer& operator=(Customer&& customer) {
    id_ = customer.id_;
    name_ = std::move(customer.name_);
    surname_ = std::move(customer.surname_);
    customer_interactions_ = std::move(customer.customer_interactions_);
    return *this;
  }

  /// @brief A customer is valid when its ID is valid.
  /// Valid IDs start at 1 while 0 is a reserved value.
  /// @return True if the ID is greater than 0
  bool IsValid() const { return id_ > INVALID_CUSTOMER_ID; }

  /// @brief Stream operator overload to easily serialize the customer into an
  /// output stream. Defined as a friend method for convenience instead of
  /// defining it into the global scope.
  /// @param os Output stream to save the data into
  /// @param customer Customer object
  /// @return Reference to output stream
  friend std::ostream& operator<<(std::ostream& os, const Customer& customer) {
    os << customer.id_ << SERIALIZATION_DELIMITER;
    os << customer.name_ << SERIALIZATION_DELIMITER;
    os << customer.surname_;
    for (const auto& interaction : customer.customer_interactions_) {
      os << SERIALIZATION_DELIMITER << *interaction;
    }
    os << std::endl;
    return os;
  }

  /// @brief Stream operator overload to easily deserialize a stream and extract
  /// all the information for a Customer. Defined as a friend method for
  /// convenience instead of defining it into the global scope.
  /// @param is Input stream to read the data from
  /// @param customer Customer object to store the extracted data into
  /// @return Reference to input stream
  friend std::istream& operator>>(std::istream& is, Customer& customer) {
    std::string id{};
    std::getline(is, id, SERIALIZATION_DELIMITER);
    utilities::try_convert(id, customer.id_);

    std::getline(is, customer.name_, SERIALIZATION_DELIMITER);
    std::getline(is, customer.surname_, SERIALIZATION_DELIMITER);

    while (!is.eof()) {
      std::shared_ptr<Interaction> interaction{std::make_shared<Interaction>()};
      if (!std::getline(is, interaction->when_, SERIALIZATION_DELIMITER) ||
          !std::getline(is, interaction->what_, SERIALIZATION_DELIMITER)) {
        break;
      }

      customer.customer_interactions_.push_back(interaction);
    }

    return is;
  }

  /// @brief Helper method to print Customer information to screen
  void PrintInfo() const {
    std::cout << id_ << ") " << name_ << " " << surname_ << std::endl;
  }

  /// @brief Checks if the user has had any interactions yet
  /// @return True if interactions are stored, false otherwise
  bool HasInteractions() const { return !customer_interactions_.empty(); }

  /// @brief Helper method to print all customer interactions to screen
  void PrintInteractions() const {
    for (const auto& interaction : customer_interactions_) {
      interaction->Print();
    }
  }
};

#endif  // __CUSTOMERS_H__
