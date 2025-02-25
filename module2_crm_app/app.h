#ifndef __APP_H__
#define __APP_H__

#include <cstdint>
#include <functional>
#include <map>

#include "crm.h"

/// @brief Handles all the user-input logic through terminal
class App {
 public:
  /// @brief Default path where the database is stored
  const char* const DATABASE_PATH = "./data.tsv";

  /// @brief Defines a list of commands for the Terminal App
  enum class ECommand : std::uint32_t {
    ADD_CUSTOMER = 1,
    SHOW_CUSTOMERS,
    EDIT_CUSTOMER,
    REMOVE_CUSTOMER,
    SEARCH_CUSTOMER,
    MANAGE_CUSTOMER_INTERACTIONS,
    EXIT,

    INVALID = UINT32_MAX,
  };

  /// @brief SubCommands for certain main commands of ECommand
  enum class ESubCommand : std::uint32_t {
    CLIENT_INTERACTIONS_ADD = 1,
    CLIENT_INTERACTIONS_SHOW,
    CLIENT_INTERACTIONS_SEARCH,
    CLIENT_INTERACTIONS_RESELECT_CLIENT,

    RETURN,

    INVALID = UINT32_MAX,
  };

  // No move and copy constructors/operators
  App(const App&) = delete;
  App& operator=(const App&) = delete;
  App(App&&) = delete;
  App& operator=(App&&) = delete;

  App();

  /// @brief Entrypoint of our App class
  /// @return Returns a status code
  std::int32_t Run();

 private:
  /// @brief Displays the main menu
  void ShowMenu();

  /// @brief Displays the submenu
  /// @param cmd Main command whose submenu shall be displayed
  void ShowSubMenu(ECommand cmd);

  /// @brief Starts the guided procedure to add a new client
  void AddClient();

  /// @brief Shows all currently saved clients
  void ShowClients() const;

  /// @brief Starts the guided procedure to edit existing clients
  void EditClient();

  /// @brief Starts the guided procedure to remove existing clients
  void RemoveClient();

  /// @brief Starts the guided procedure to search for clients
  void SearchClient();

  /// @brief Starts the guided procedure to manage clients' interactions
  void ManageClientInteractions();

  /// @brief Starts the guided procedure to add new interactions
  /// to the selected client
  void AddClientInteraction();

  /// @brief Starts the guided procedure to search for interactions
  /// on the selected client in a user-defined time interval
  void SearchClientInteractions();

  /// @brief Shows all interactions of the currently selected client
  void ShowClientInteractions();

  /// @brief Resets the currently selected client and returns to the
  /// interactions management menu
  void ReselectClientForInteractions();

  /// @brief Starts the guided procedure to find and select clients
  /// based on ID, Name and/or Surname
  /// @param output_id Selected client id
  /// @param no_selection Whether to guide the user through the selection
  /// of a client when the search yields more than 1 result
  /// @return True if client successfully selected, false otherwise
  bool FindAndSelectClient(Customer::ID& output_id,
                           const bool no_selection = false) const;

  /// @brief Manager class that directly interfaces the database
  CRM customer_manager_;

  /// @brief Helper structure to store menu options
  struct CommandData {
    const char* description_;
    std::function<void()> callback_;
    std::map<ESubCommand, CommandData> submenu_;

    CommandData() = default;
    CommandData(const char* desc, const std::function<void()>& cb)
        : description_{desc}, callback_{cb}, submenu_{} {}

    void AddSubMenu(ESubCommand cmd, const char* desc,
                    const std::function<void()>& cb) {
      submenu_[cmd] = CommandData{desc, cb};
    }
  };
  /// @brief Stores all available commands and corresponding descriptions
  /// and callbacks for dynamic menu generation
  std::map<ECommand, CommandData> commands_;

  /// @brief Client ID selected during Interaction management
  Customer::ID managed_customer_id_;
};

#endif  // __APP_H__
