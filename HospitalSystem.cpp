#include <iostream>
#include <queue>
#include <stack>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

struct Patient {
    int id;
    string name;
    int age;
    string gender;
    string condition;
    string treatmentHistory;
    Patient* next;
    int triageLevel; // 1 = Critical, 2 = Serious, 3 = Stable
    bool inER;

    Patient(int i, string n, int a, string g, string c) {
        id = i;
        name = n;
        age = a;
        gender = g;
        condition = c;
        treatmentHistory = "";
        next = nullptr;
        triageLevel = 0;
        inER = false;
    }
};

class HospitalSystem {
private:
    Patient* head;
    vector<Patient*> erPatients;
    stack<pair<Patient*, string>> treatmentStack;
    int nextID;

public:
    HospitalSystem() {
        head = nullptr;
        nextID = 1;
    }

    void addPatient() {
    string name, condition, gender;
    int age;

    cout << ">> Enter patient name: ";
    cin >> ws;
    getline(cin, name);

    cout << ">> Enter gender (M/F): ";
    getline(cin, gender);

    cout << ">> Enter age: ";
    while (!(cin >> age)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << ">> Invalid input. Please enter a number for age: ";
    }
    cin.ignore();

    cout << ">> Enter condition: ";
    getline(cin, condition);

    Patient* newPatient = new Patient(nextID++, name, age, gender, condition);
    if (head == nullptr) {
        head = newPatient;
    } else {
        Patient* temp = head;
        while (temp->next != nullptr)
            temp = temp->next;
        temp->next = newPatient;
    }
    cout << ">> Patient added.\n";
}

    void displayPatients() {
        if (head == nullptr) {
            cout << ">> No patient records found.\n";
            return;
        }
        Patient* temp = head;
        cout << "\n==============================\n";
        cout << "       PATIENT RECORDS\n";
        cout << "==============================\n";
        while (temp != nullptr) {
            cout << "ID: " << temp->id << ", Name: " << temp->name
                 << ", Age: " << temp->age << ", Gender: " << temp->gender << ", Condition: " << temp->condition
                 << ", In ER: " << (temp->inER ? "Yes" : "No")
                 << ", Treatment History: " << (temp->treatmentHistory.empty() ? "No treatments yet" : temp->treatmentHistory) << endl;
            temp = temp->next;
        }
    }

    Patient* findPatientByID(int id) {
        Patient* temp = head;
        while (temp != nullptr) {
            if (temp->id == id)
                return temp;
            temp = temp->next;
        }
        return nullptr;
    }

    void addToER() {
        int id;
        cout << ">> Enter patient ID to send to ER: ";
        cin >> id;

        Patient* p = findPatientByID(id);
        if (p == nullptr) {
            cout << ">> Patient not found.\n";
            return;
        }

        if (p->inER) {
            cout << ">> Patient is already in the ER.\n";
            return;
        }

        int level;
        cout << "\n>> Select Triage Level for " << p->name << ":\n";
        cout << "   1 >> Critical\n";
        cout << "   2 >> Serious\n";
        cout << "   3 >> Stable\n";
        cout << ">> Enter triage level: ";
        cin >> level;

        if (level < 1 || level > 3) {
            cout << ">> Invalid triage level. Operation cancelled.\n";
            return;
        }

        p->triageLevel = level;
        p->inER = true;
        erPatients.push_back(p);
        cout << ">> " << p->name << " added to ER with triage level " << level << ".\n";
    }

    void treatERPatient() {
        if (erPatients.empty()) {
            cout << ">> No patients in the ER.\n";
            return;
        }

        cout << "\n>> ER Patients:\n";
        for (Patient* p : erPatients) {
            cout << "ID: " << p->id << ", Name: " << p->name
                 << ", Triage Level: " << p->triageLevel
                 << ", Condition: " << p->condition << endl;
        }

        int id;
        cout << "\n>> Enter patient ID to treat: ";
        cin >> id;

        auto it = find_if(erPatients.begin(), erPatients.end(), [id](Patient* p) {
            return p->id == id;
        });

        if (it == erPatients.end()) {
            cout << ">> Patient not found in ER.\n";
            return;
        }

        Patient* p = *it;
        string treatment;
        cout << ">> Enter treatment for " << p->name << ": ";
        cin.ignore();
        getline(cin, treatment);

        p->treatmentHistory += (p->treatmentHistory.empty() ? "" : ", ") + treatment;
        treatmentStack.push({p, treatment});

        p->inER = false;
        erPatients.erase(it);

        cout << ">> Treatment complete. " << p->name << " has been removed from ER.\n";
    }

    void treatRegularPatient() {
        vector<Patient*> regularPatients;
        Patient* temp = head;

        cout << "\n>> Regular Patients:\n";
        while (temp != nullptr) {
            if (!temp->inER) {
                cout << "ID: " << temp->id << ", Name: " << temp->name
                     << ", Condition: " << temp->condition << endl;
                regularPatients.push_back(temp);
            }
            temp = temp->next;
        }

        if (regularPatients.empty()) {
            cout << ">> No regular patients available.\n";
            return;
        }

        int id;
        cout << "\n>> Enter patient ID to treat: ";
        cin >> id;

        Patient* p = findPatientByID(id);
        if (p == nullptr || p->inER) {
            cout << ">> Invalid patient or currently in ER.\n";
            return;
        }

        string treatment;
        cout << ">> Enter treatment: ";
        cin.ignore();
        getline(cin, treatment);

        p->treatmentHistory += (p->treatmentHistory.empty() ? "" : ", ") + treatment;
        treatmentStack.push({p, treatment});

        cout << ">> Treatment added for " << p->name << ".\n";
    }

    void undoLastTreatment() {
        if (treatmentStack.empty()) {
            cout << ">> No treatment history.\n";
            return;
        }

        auto last = treatmentStack.top();
        Patient* p = last.first;
        string treatment = last.second;
        treatmentStack.pop();

        string& history = p->treatmentHistory;
        size_t pos = history.find(treatment);

        if (pos != string::npos) {
            if (pos >= 2 && history.substr(pos - 2, 2) == ", ") {
                history.erase(pos - 2, treatment.length() + 2);
            } else if (pos + treatment.length() < history.length() &&
                       history.substr(pos + treatment.length(), 2) == ", ") {
                history.erase(pos, treatment.length() + 2);
            } else {
                history.erase(pos, treatment.length());
            }

            if (!history.empty() && history.front() == ' ')
                history.erase(0, 1);
            if (!history.empty() && history.back() == ' ')
                history.pop_back();
            if (!history.empty() && history.front() == ',')
                history.erase(0, 1);
            if (!history.empty() && history.back() == ',')
                history.pop_back();
        }

        cout << ">> Treatment '" << treatment << "' removed from " << p->name << ".\n";
    }

    void searchPatient() {
        string name;
        cout << ">> Enter patient name to search: ";
        cin >> ws;
        getline(cin, name);

        Patient* temp = head;
        while (temp != nullptr) {
            if (temp->name == name) {
                cout << "\n>> Patient found: ID: " << temp->id << ", Name: " << temp->name
                     << ", Age: " << temp->age << ", Gender: " << temp->gender << ", Condition: " << temp->condition
                     << ", In ER: " << (temp->inER ? "Yes" : "No")
                     << ", Treatment History: " << (temp->treatmentHistory.empty() ? "No treatments yet" : temp->treatmentHistory) << endl;
                return;
            }
            temp = temp->next;
        }

        cout << ">> Patient not found.\n";
    }

    void viewERQueue() {
        if (erPatients.empty()) {
            cout << ">> No patients in the ER queue.\n";
            return;
        }

        cout << "\n>> ER Queue:\n";
        for (Patient* p : erPatients) {
            cout << "ID: " << p->id << ", Name: " << p->name
                 << ", Triage Level: " << p->triageLevel
                 << ", Condition: " << p->condition << endl;
        }
    }

    void run() {
        int choice;
        do {
            cout << "\n==============================\n";
            cout << "       HOSPITAL SYSTEM\n";
            cout << "==============================\n";
            cout << "1 >> Add New Patient\n";
            cout << "2 >> View All Patients\n";
            cout << "3 >> Send Patient to ER\n";
            cout << "4 >> Treat ER Patient\n";
            cout << "5 >> Treat Regular Patient\n";
            cout << "6 >> Undo Last Treatment\n";
            cout << "7 >> Search Patient by Name\n";
            cout << "8 >> View ER Queue\n";
            cout << "0 >> Exit\n";
            cout << "==============================\n";
            cout << ">> Enter choice: ";
            cin >> choice;

            switch (choice) {
                case 1: addPatient(); break;
                case 2: displayPatients(); break;
                case 3: addToER(); break;
                case 4: treatERPatient(); break;
                case 5: treatRegularPatient(); break;
                case 6: undoLastTreatment(); break;
                case 7: searchPatient(); break;
                case 8: viewERQueue(); break;
                case 0: cout << ">> Exiting...\n"; break;
                default: cout << ">> Invalid choice.\n";
            }
        } while (choice != 0);
    }
};

int main() {
    HospitalSystem hs;
    hs.run();
    return 0;
}