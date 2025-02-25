#include "app.h"

#include <ctime>
#include <iostream>
#include <string>

#include "utilities.h"

namespace {

// Source: https://stackoverflow.com/questions/17335816/clear-screen-using-c
// Intended to clear the screen in a way that works both on UNIX and Win32
void clear_screen() { std::cout << "\033[2J\033[1;1H"; }

/// @brief Helper method to fetch user input from console
/// @param message A prompt message to display
/// @return User-input from console
std::string prompt_user_input(const char* message) {
  std::cout << message;
  std::string terminal_input{};
  std::getline(std::cin, terminal_input);

  return terminal_input;
}

/// @brief Helper method to convert a string to its corresponding Enum value
/// @tparam EnumT Enum class to convert to
/// @tparam min_value Min. value to consider the input valid
/// @tparam max_value Max. value to consider the input valid
/// @param str String to convert
/// @return Enum value on success, INVALID on failure
template <typename EnumT, EnumT min_value, EnumT max_value>
EnumT to_enum(const std::string& str) {
  std::uint32_t value{static_cast<std::uint32_t>(EnumT::INVALID)};
  utilities::try_convert(str, value);

  if (value >= static_cast<std::uint32_t>(min_value) &&
      value <= static_cast<std::uint32_t>(max_value)) {
    return static_cast<EnumT>(value);
  }

  return EnumT::INVALID;
}

}  // namespace

App::App() : customer_manager_{DATABASE_PATH}, managed_customer_id_{} {
  commands_ = {
      {ECommand::ADD_CUSTOMER,
       {"Aggiungi un nuovo Cliente", std::bind(&App::AddClient, this)}},
      {ECommand::SHOW_CUSTOMERS,
       {"Visualizza tutti i Clienti", std::bind(&App::ShowClients, this)}},
      {ECommand::EDIT_CUSTOMER,
       {"Modifica un Cliente", std::bind(&App::EditClient, this)}},
      {ECommand::REMOVE_CUSTOMER,
       {"Rimuovi un Cliente", std::bind(&App::RemoveClient, this)}},
      {ECommand::SEARCH_CUSTOMER,
       {"Cerca un Cliente", std::bind(&App::SearchClient, this)}},
      {ECommand::MANAGE_CUSTOMER_INTERACTIONS,
       {"Gestisci interazioni",
        std::bind(&App::ManageClientInteractions, this)}},
      {ECommand::EXIT, {"Chiudi", []() { return false; }}},
  };

  auto& manage_interactions = commands_[ECommand::MANAGE_CUSTOMER_INTERACTIONS];
  manage_interactions.AddSubMenu(ESubCommand::CLIENT_INTERACTIONS_ADD,
                                 "Aggiungi interazione",
                                 std::bind(&App::AddClientInteraction, this));
  manage_interactions.AddSubMenu(ESubCommand::CLIENT_INTERACTIONS_SHOW,
                                 "Visualizza interazioni",
                                 std::bind(&App::ShowClientInteractions, this));
  manage_interactions.AddSubMenu(
      ESubCommand::CLIENT_INTERACTIONS_SEARCH, "Cerca interazioni",
      std::bind(&App::SearchClientInteractions, this));
  manage_interactions.AddSubMenu(
      ESubCommand::CLIENT_INTERACTIONS_RESELECT_CLIENT,
      "Seleziona nuovo cliente da gestire",
      std::bind(&App::ReselectClientForInteractions, this));
  manage_interactions.AddSubMenu(ESubCommand::RETURN,
                                 "Torna alla pagina principale",
                                 []() { return false; });
}

std::int32_t App::Run() {
  while (true) {
    managed_customer_id_ = 0;

    std::cout << "CRM per InsuraPro Solutions!" << std::endl;
    ShowMenu();

    const std::string action = prompt_user_input("Cosa vuoi fare? ");
    const ECommand selected_action =
        to_enum<App::ECommand, App::ECommand::ADD_CUSTOMER,
                App::ECommand::EXIT>(action);

    clear_screen();

    if (selected_action == ECommand::INVALID) {
      std::cout << "L'azione scelta non è valida." << std::endl << std::endl;
      continue;
    }

    if (selected_action == ECommand::EXIT) {
      break;
    }

    commands_[selected_action].callback_();
    std::cout << std::endl;
  }

  return EXIT_SUCCESS;
}

void App::ShowMenu() {
  for (const auto& command : commands_) {
    std::cout << static_cast<std::uint32_t>(command.first) << ") "
              << command.second.description_ << std::endl;
  }
}

void App::ShowSubMenu(ECommand cmd) {
  for (const auto& command : commands_[cmd].submenu_) {
    std::cout << static_cast<std::uint32_t>(command.first) << ") "
              << command.second.description_ << std::endl;
  }
}

void App::AddClient() {
  std::cout << "Aggiungi nuovo cliente" << std::endl;
  std::cout << "Compila i campi di seguito oppure lasciali vuoti per tornare "
               "indietro."
            << std::endl;
  const std::string name = prompt_user_input("Nome: ");
  const std::string surname = prompt_user_input("Cognome: ");

  if (name.empty() && surname.empty()) {
    return;
  } else if (name.empty() || surname.empty()) {
    clear_screen();

    std::cout << "Per aggiungere un cliente devi compilare entrambi i campi!"
              << std::endl
              << std::endl;
    AddClient();
    return;
  }

  if (customer_manager_.AddCustomer(name, surname)) {
    std::cout << "Cliente aggiunto." << std::endl;
  } else {
    std::cout << "Il cliente esiste già!" << std::endl;
  }
}

void App::ShowClients() const {
  std::cout << "Visualizza tutti i clienti" << std::endl;
  if (!customer_manager_.PrintAllCustomers()) {
    std::cout << "Non ci sono clienti." << std::endl;
    return;
  }

  prompt_user_input("Premere invio per tornare alla schermata iniziale.");
}

void App::EditClient() {
  std::cout << "Modifica di un cliente" << std::endl;
  Customer::ID selected_customer_id{};

  if (!FindAndSelectClient(selected_customer_id)) {
    std::cout << "Operazione annullata." << std::endl;
    return;
  }

  std::cout << std::endl;

  const auto& selected_customer =
      customer_manager_.GetCustomer(selected_customer_id);

  std::cout << "Utente selezionato: " << std::endl;
  selected_customer.PrintInfo();
  std::cout << std::endl;

  std::cout << "Compila i campi di seguito, o lasciali vuoti per non apportare "
               "modifiche:"
            << std::endl;
  const std::string new_name = prompt_user_input("Nome: ");
  const std::string new_surname = prompt_user_input("Cognome: ");

  if (new_name.empty() && new_surname.empty()) {
    std::cout << "Non sono state apportate modifiche al cliente selezionato."
              << std::endl;
    return;
  }

  if (customer_manager_.UpdateClientInfo(
          selected_customer_id,
          new_name.empty() ? selected_customer.name_ : new_name,
          new_surname.empty() ? selected_customer.surname_ : new_surname)) {
    std::cout << "Modifiche apportate con successo." << std::endl;
  } else {
    std::cout << "Si è verificato un errore durante il salvataggio."
              << std::endl;
  }
}

void App::RemoveClient() {
  std::cout << "Rimuovi un cliente" << std::endl;
  Customer::ID selected_customer_id{};

  if (!FindAndSelectClient(selected_customer_id)) {
    std::cout << "Operazione annullata." << std::endl;
    return;
  }

  std::cout << "Sei sicuro di voler rimuovere il cliente selezionato?"
            << std::endl;
  std::string confirm =
      prompt_user_input("L'operazione sarà irreversibile! [Si/No] ");

  if (!confirm.empty() && (confirm[0] == 's' || confirm[0] == 'S')) {
    if (!customer_manager_.RemoveCustomer(selected_customer_id)) {
      std::cout << "Impossibile rimuovere il cliente selezionato." << std::endl;
      return;
    } else {
      std::cout << "Cliente rimosso." << std::endl;
    }
  } else {
    std::cout << "Operazione annullata." << std::endl;
  }
}

void App::SearchClient() {
  std::cout << "Cerca un cliente" << std::endl;
  Customer::ID selected_customer_id{};
  if (!FindAndSelectClient(selected_customer_id, true)) {
    return;
  }

  const auto& customer = customer_manager_.GetCustomer(selected_customer_id);
  customer.PrintInfo();
  std::cout << std::endl;
}

void App::ManageClientInteractions() {
  while (true) {
    if (managed_customer_id_ == 0) {
      std::cout << "Prima di procedere è necessario selezionare un Cliente da "
                   "gestire."
                << std::endl;

      if (!FindAndSelectClient(managed_customer_id_)) {
        std::cout
            << "Devi selezionare un cliente per poter gestire le interazioni."
            << std::endl;
        return;
      }

      clear_screen();
    }

    std::cout << "Gestione delle Interazioni" << std::endl;

    const auto& selected_customer =
        customer_manager_.GetCustomer(managed_customer_id_);
    std::cout << "Cliente selezionato: " << std::endl;
    selected_customer.PrintInfo();
    std::cout << std::endl;

    ShowSubMenu(ECommand::MANAGE_CUSTOMER_INTERACTIONS);

    const std::string action = prompt_user_input("Cosa vuoi fare? ");
    const ESubCommand selected_action =
        to_enum<App::ESubCommand, App::ESubCommand::CLIENT_INTERACTIONS_ADD,
                App::ESubCommand::RETURN>(action);

    clear_screen();

    if (selected_action == ESubCommand::INVALID) {
      std::cout << "L'azione scelta non è valida." << std::endl << std::endl;
      continue;
    }

    if (selected_action == ESubCommand::RETURN) {
      break;
    }

    commands_[ECommand::MANAGE_CUSTOMER_INTERACTIONS]
        .submenu_[selected_action]
        .callback_();
    std::cout << std::endl;
  }
}

void App::AddClientInteraction() {
  std::cout << "Aggiungi nuova interazione" << std::endl;
  std::cout << "Lasciare entrambi i campi vuoti per tornare al menu."
            << std::endl;
  std::string when =
      prompt_user_input("Data dell'interazione (ad es.: 15/12/2024 16:15) ");

  if (!utilities::is_valid_date(when, DATE_FORMAT)) {
    clear_screen();

    std::cout << "La data inserita non è valida!" << std::endl << std::endl;
    AddClientInteraction();
    return;
  }

  std::string what = prompt_user_input("Breve descrizione: ");
  std::cout << std::endl;

  utilities::remove_chars_from_str(what, "\t\r\n", ' ');

  if (when.empty() && what.empty()) {
    return;
  } else if (when.empty() || what.empty()) {
    clear_screen();

    std::cout << "Entrambi i campi sono obbligatori!" << std::endl;
    AddClientInteraction();
    return;
  }

  std::string confirm = prompt_user_input("Salvare l'interazione? [Si/No] ");
  if (!confirm.empty() && (confirm[0] == 's' || confirm[0] == 'S')) {
    if (customer_manager_.AddInteraction(managed_customer_id_, when, what)) {
      std::cout << "Interazione aggiunta con successo." << std::endl;
    } else {
      std::cout << "Si è verificato un errore e non è stato possibile "
                   "completare la richiesta."
                << std::endl;
    }
  }
}

void App::SearchClientInteractions() {
  std::cout << "Cerca interazione" << std::endl;
  std::cout << "Inserisci le date nell'intervallo in cui cercare. (Formato: "
               "Giorno/Mese/Anno)"
            << std::endl;
  std::string from_date = prompt_user_input("Dal: ");
  std::string to_date = prompt_user_input("Al: ");

  std::time_t from_timestamp{};
  std::time_t to_timestamp{};
  if (!utilities::to_timestamp(from_date, "%d/%m/%Y", from_timestamp) ||
      !utilities::to_timestamp(to_date, "%d/%m/%Y", to_timestamp)) {
    std::cout << "Le date inserite non sono nel formato corretto." << std::endl;
    return;
  }

  if (from_timestamp > to_timestamp) {
    std::swap(from_timestamp, to_timestamp);
  }

  std::cout << std::endl;
  if (!customer_manager_.PrintCustomerInteractions(
          managed_customer_id_, from_timestamp, to_timestamp)) {
    std::cout << "Non sono state trovate interazioni nel periodo specificato."
              << std::endl;
  }

  prompt_user_input("Premere invio per tornare alla schermata iniziale.");
}

void App::ShowClientInteractions() {
  std::cout << "Visualizza interazioni" << std::endl;
  const auto& customer = customer_manager_.GetCustomer(managed_customer_id_);
  if (!customer.HasInteractions()) {
    std::cout << "Non ci sono interazioni registrate per l'attuale cliente."
              << std::endl;
    return;
  }

  customer.PrintInteractions();

  prompt_user_input("Premere invio per tornare alla schermata iniziale.");
}

void App::ReselectClientForInteractions() { managed_customer_id_ = 0; }

bool App::FindAndSelectClient(Customer::ID& output_id,
                              const bool no_selection) const {
  std::cout << "Puoi specificare uno o più campi per affinare la ricerca "
            << std::endl;
  std::string id = prompt_user_input("ID Cliente (Opzionale): ");
  std::string name{};
  std::string surname{};

  if (id.empty()) {
    name = prompt_user_input("Nome (Opzionale): ");
    surname = prompt_user_input("Cognome (Opzionale): ");
  }

  std::vector<Customer::ID> found_customers{};

  if (customer_manager_.FindCustomers(id, name, surname, found_customers)) {
    if (found_customers.size() > 1) {
      std::cout << "Trovate " << found_customers.size() << " corrispondenze."
                << std::endl;
      customer_manager_.PrintCustomersByID(found_customers);

      if (no_selection) {
        return false;
      }

      const std::string client_id = prompt_user_input(
          "Seleziona un ID Cliente o digita 'annulla' per tornare al "
          "menu principale: ");

      if (client_id == "annulla") {
        return false;
      } else if (!utilities::try_convert(client_id, output_id) ||
                 !utilities::is_in_vector(found_customers, output_id)) {
        std::cout << "ID Cliente non valido. Si prega di selezionare un ID "
                     "presente nelle corrispondenze della ricerca."
                  << std::endl;

        clear_screen();
        return FindAndSelectClient(output_id);
      }
    } else {
      output_id = found_customers[0];
    }

    return true;
  }

  std::cout << std::endl
            << "La ricerca non ha prodotto alcun risultato." << std::endl;
  return false;
}