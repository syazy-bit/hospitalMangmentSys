#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdio> // for remove, rename
using namespace std;

// Doctor class
class Doctor
{
public:
    string name;
    string specialization;

    Doctor(string n, string s)
    {
        name = n;
        specialization = s;
    }
};

// Predefined doctor list
vector<Doctor> doctors = {
    Doctor("Dr. Sharma", "Cardiologist"),   // Heart & Stroke
    Doctor("Dr. Gupta", "Surgeon"),         // Accidents, Injuries
    Doctor("Dr. Khan", "Pediatrician"),     // Child, Flu
    Doctor("Dr. Das", "General Physician"), // General cases
    Doctor("Dr. Mehta", "Oncologist"),      // Cancer
    Doctor("Dr. Rao", "Endocrinologist"),   // Diabetes
    Doctor("Dr. Iqbal", "Pulmonologist")    // Asthma, Lungs
};

// Patient class
class Patient
{
public:
    int id;
    string name;
    int age;
    string gender;
    string disease;
    string doctor;
    string ward;
    int roomNo;
    int fees;
    string status; // ✅ New field for patient status

    // Convert patient to string for file storage
    string toString()
    {
        return to_string(id) + "," + name + "," + to_string(age) + "," + gender + "," + disease + "," +
               doctor + "," + ward + "," + to_string(roomNo) + "," + to_string(fees) + "," + status;
    }

    // Create patient object from a line
    static Patient fromString(const std::string &line)
    {
        Patient p;
        size_t pos = 0, prev = 0;
        vector<string> tokens;

        while ((pos = line.find(",", prev)) != std::string::npos)
        {
            tokens.push_back(line.substr(prev, pos - prev));
            prev = pos + 1;
        }
        tokens.push_back(line.substr(prev));

        if (tokens.size() >= 10)
        {
            p.id = stoi(tokens[0]);
            p.name = tokens[1];
            p.age = stoi(tokens[2]);
            p.gender = tokens[3];
            p.disease = tokens[4];
            p.doctor = tokens[5];
            p.ward = tokens[6];
            p.roomNo = stoi(tokens[7]);
            p.fees = stoi(tokens[8]);
            p.status = tokens[9];
        }
        return p;
    }

    void display()
    {
        cout << "ID: " << id << "\n"
             << "Name: " << name << "\n"
             << "Age: " << age << "\n"
             << "Gender: " << gender << "\n"
             << "Disease: " << disease << "\n"
             << "Doctor: " << doctor << "\n"
             << "Ward: " << ward << "\n"
             << "Room No: " << roomNo << "\n"
             << "Fees: " << fees << " INR\n"
             << "Status: " << status << "\n"
             << "-----------------------\n";
    }
};

// Disease → Specialization mapping
string assignDoctorByDisease(const string &disease)
{
    string d = disease;
    for (auto &c : d)
        c = tolower(c); // lowercase for easy matching

    if (d.find("heart") != string::npos || d.find("stroke") != string::npos)
        return "Cardiologist";
    if (d.find("accident") != string::npos || d.find("injury") != string::npos || d.find("critical") != string::npos)
        return "Surgeon";
    if (d.find("child") != string::npos || d.find("fever") != string::npos || d.find("flu") != string::npos)
        return "Pediatrician";
    if (d.find("cancer") != string::npos || d.find("tumor") != string::npos)
        return "Oncologist";
    if (d.find("diabetes") != string::npos || d.find("sugar") != string::npos)
        return "Endocrinologist";
    if (d.find("asthma") != string::npos || d.find("lungs") != string::npos || d.find("breath") != string::npos)
        return "Pulmonologist";

    return "General Physician";
}

// Get doctor name from specialization
string getDoctor(const string &disease)
{
    string needed = assignDoctorByDisease(disease);
    for (auto &doc : doctors)
    {
        if (doc.specialization == needed)
        {
            return doc.name + " (" + doc.specialization + ")";
        }
    }
    return "Dr. Das (General Physician)";
}

// Assign ward and room
pair<string, int> assignWardAndRoom(const string &disease, int patientId)
{
    if (disease.find("accident") != string::npos || disease.find("critical") != string::npos || disease.find("injury") != string::npos)
    {
        return {"Emergency", (patientId % 5) + 1}; // Emergency room numbers 1–5
    }
    return {"General", (patientId % 5) + 1}; // General room numbers 1–5
}

// Calculate fees
int calculateFees(const string &ward)
{
    if (ward == "Emergency")
        return 5000;
    return 2000;
}

// Add Patient
void addPatient()
{
    Patient p;
    cout << "Enter ID: ";
    cin >> p.id;
    cin.ignore();
    cout << "Enter Name: ";
    getline(cin, p.name);
    cout << "Enter Age: ";
    cin >> p.age;
    cin.ignore();
    cout << "Enter Gender: ";
    getline(cin, p.gender);
    cout << "Enter Disease: ";
    getline(cin, p.disease);

    // Auto-assign doctor, ward, room, fees
    p.doctor = getDoctor(p.disease);
    auto wardRoom = assignWardAndRoom(p.disease, p.id);
    p.ward = wardRoom.first;
    p.roomNo = wardRoom.second;
    p.fees = calculateFees(p.ward);
    p.status = "Admitted"; // ✅ default status

    ofstream fout("patients.txt", ios::app);
    fout << p.toString() << "\n";
    fout.close();

    cout << "✅ Patient added successfully!\n";
    p.display();
}

// View Patients
void viewPatients()
{
    ifstream fin("patients.txt");
    string line;
    cout << "\n---- All Patients ----\n";
    while (getline(fin, line))
    {
        Patient p = Patient::fromString(line);
        p.display();
    }
    fin.close();
}

// Search Patient
void searchPatient()
{
    int searchId;
    cout << "Enter Patient ID to search: ";
    cin >> searchId;

    ifstream fin("patients.txt");
    string line;
    bool found = false;
    while (getline(fin, line))
    {
        Patient p = Patient::fromString(line);
        if (p.id == searchId)
        {
            cout << "\n✅ Patient Found:\n";
            p.display();
            found = true;
            break;
        }
    }
    fin.close();

    if (!found)
        cout << "❌ Patient not found.\n";
}

// Update Patient Status
void updateStatus()
{
    int updateId;
    cout << "Enter Patient ID to update status: ";
    cin >> updateId;

    ifstream fin("patients.txt");
    ofstream fout("temp.txt");

    string line;
    bool updated = false;

    while (getline(fin, line))
    {
        Patient p = Patient::fromString(line);
        if (p.id == updateId)
        {
            cout << "Enter new status (Admitted/Discharged/Under Treatment): ";
            cin.ignore();
            getline(cin, p.status);
            updated = true;
        }
        fout << p.toString() << "\n";
    }

    fin.close();
    fout.close();

    ::remove("patients.txt");
    ::rename("temp.txt", "patients.txt");

    if (updated)
        cout << "✅ Patient status updated successfully!\n";
    else
        cout << "❌ Patient not found.\n";
}

// Main Menu
int main()
{
    int choice;
    do
    {
        cout << "\n---- Hospital Management System ----\n";
        cout << "1. Add Patient\n";
        cout << "2. View All Patients\n";
        cout << "3. Search Patient\n";
        cout << "4. Update Patient Status\n"; // ✅ replaced delete
        cout << "5. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice)
        {
        case 1:
            addPatient();
            break;
        case 2:
            viewPatients();
            break;
        case 3:
            searchPatient();
            break;
        case 4:
            updateStatus();
            break;
        case 5:
            cout << "Exiting...\n";
            break;
        default:
            cout << "Invalid choice. Try again.\n";
        }
    } while (choice != 5);

    return 0;
}
