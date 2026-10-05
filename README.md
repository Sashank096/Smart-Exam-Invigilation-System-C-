<div align="center">


# 🎓 Smart Exam Invigilation System

**A C++ desktop application that simplifies examination management — students, invigilators, halls, exam schedules and conflict-free invigilator allocation, all in one place.**

![C++](https://img.shields.io/badge/C++-17-00599C?style=for-the-badge&logo=cplusplus&logoColor=white)
![SFML](https://img.shields.io/badge/GUI-SFML%202.6.1-8CC445?style=for-the-badge)
![OOP](https://img.shields.io/badge/Concept-OOP-1565C0?style=for-the-badge)
![Storage](https://img.shields.io/badge/Storage-CSV%20Files-2EA043?style=for-the-badge)
![Platform](https://img.shields.io/badge/Platform-Windows-0078D6?style=for-the-badge&logo=windows&logoColor=white)

</div>

---

## 📑 Table of Contents

- [About the Project](#-about-the-project)
- [Problem Statement](#-problem-statement)
- [Objectives](#-objectives)
- [Key Features](#-key-features)
- [Tech Stack](#-tech-stack)
- [System Architecture](#-system-architecture)
- [Modules & Screenshots](#-modules--screenshots)
- [Project Demonstration Flow](#-project-demonstration-flow)
- [C++ OOP Concepts Used](#-c-oop-concepts-used)
- [Project Structure](#-project-structure)
- [Getting Started](#-getting-started)
- [Default Login](#-default-login)
- [Testing](#-testing)


---

## 📖 About the Project

The **Smart Exam Invigilation System** is a C++ application built to take the paperwork out of examination management. It gives the administrator a **single, centralized place** to manage:

| | Area | What it covers |
|---|---|---|
| 👨‍🎓 | **Students** | Student records and hall numbers |
| 👨‍🏫 | **Invigilators** | Invigilator details and availability |
| 🏫 | **Exam Halls** | Hall numbers, blocks, capacity and availability |
| 📅 | **Exam Schedules** | Subjects, dates, timings and departments |
| 📋 | **Invigilator Allocation** | Assigning invigilators to halls, with conflict checking |
| 📊 | **Reports & Dashboard** | Management summaries and exportable report |

The project uses **Object-Oriented Programming (OOP)** in C++, a graphical interface built with **SFML**, and **CSV file handling** for data storage — so no database setup is needed.

---

## ❗ Problem Statement

Manual examination management is time-consuming and prone to errors, especially when maintaining records, assigning invigilators and managing examination halls. A centralized digital system is required to simplify and automate these activities.

**Key problems this project solves:**

- 📝 Manual record management
- 🔀 Difficulty in invigilator allocation
- 🏫 Hall management issues
- 📄 Increased paperwork
- ⚠️ Human errors
- 🗂️ No centralized system

---

## 🎯 Objectives

- Develop a simple and user-friendly exam invigilation system using C++
- Manage student details efficiently
- Maintain invigilator information in an organized manner
- Store and manage examination hall details
- Manage exam schedules and examination information
- Reduce manual work, paperwork and human errors
- Apply C++ OOP concepts — classes, objects, constructors and file handling

---

## ✨ Key Features

- 🔐 **Secure admin login** with a password-reset option
- 👨‍🎓 **Full CRUD** (Add / View / Search / Update / Delete) for students, invigilators, halls and exams
- 🧠 **Smart allocation** — an invigilator cannot be double-booked at an overlapping date and time, and a hall/exam pair cannot get two invigilators
- 📊 **Dashboard & reports** with live totals and an **Export Report** button (plain-text report)
- 💾 **Automatic data persistence** in CSV files (created on first run)
- ⚙️ **Admin-configurable data** — halls, subjects and exam timings are entered by the admin, not hard-coded
- 🖥️ **Clean SFML GUI** with a consistent theme across all screens

---

## 🛠️ Tech Stack

| Category | Technology |
|---|---|
| 💻 Programming Language | **C++** |
| 🧩 Programming Concept | Object-Oriented Programming (OOP) |
| 🖥️ GUI / Graphics | **SFML 2.6.1** |
| 💾 Data Storage | Text / CSV files |
| 🛠️ Compiler / IDE | Visual Studio (MSVC) / Code::Blocks |
| 🚀 Build Launcher | Python (`app.py`, launcher only) |
| 📐 Design | UML Class Diagram |

---

## 🏗️ System Architecture

<div align="center">

</div>

**Class responsibilities**

| Class | Responsibility |
|---|---|
| `Admin` | Login, logout, password reset |
| `Student` | Student record management |
| `Invigilator` | Invigilator details & availability |
| `ExamHall` | Hall details & availability |
| `Exam` | Exam schedule & subject details |
| `Allocation` | Invigilator assignment & conflict checking |
| `Report` | Reports and summaries |

---

## 🖼️ Modules & Screenshots

The system is made of **7 modules**. Each one is shown below with its purpose, main class, functions and a screenshot.

### 🔐 1. Login & Authentication

Provides secure access for authorized administrators and validates the user before entering the system.

- **Main class:** `Admin`
- **Functions:** `login()` · `logout()` · `resetPassword()`
- **Libraries:** `<string>` (string handling) · `<fstream>` (file handling) · `<utility>` (`std::move()`)

```cpp
bool login(const std::string& user,
           const std::string& pass) const {
    return user == username &&
           pass == password;
}
```

---

### 👨‍🎓 2. Student Management

Add, view, search, update and delete student records.

- **Main class:** `Student`
- **Functions:** `addStudent()` · `viewStudents()` · `searchStudent()` · `updateStudent()` · `deleteStudent()`
- **Fields:** Student ID, Name, Department, Year, Hall Number, Email, Phone
- **Flow:** `Student Details → Add/Update → Store → View/Search`
- Duplicate student IDs are rejected.

---

### 👨‍🏫 3. Invigilator Management

Manage invigilator records, contact details and availability.


- **Main class:** `Invigilator`
- **Functions:** `addInvigilator()` · `viewInvigilators()` · `searchInvigilator()` · `updateInvigilator()` · `deleteInvigilator()`
- **Fields:** Invigilator ID, Name, Department, Phone, Email, Availability
- **Flow:** `Invigilator Details → Manage → Store → View/Search`

---

### 🏫 4. Exam Hall Management

Manage hall numbers, blocks, capacity and availability.

- **Main class:** `ExamHall`
- **Functions:** `addExamHall()` · `viewExamHalls()` · `searchExamHall()` · `updateExamHall()` · `deleteExamHall()`
- **Fields:** Hall ID, Hall Number, Block, Capacity, Availability
- **Flow:** `Hall Details → Manage → Store → View/Search`

---

### 📅 5. Exam Schedule

Manage subjects, exam dates, timings and departments.


- **Main class:** `Exam`
- **Functions:** `addExam()` · `viewExams()` · `searchExam()` · `updateExam()` · `deleteExam()`
- **Fields:** Exam ID, Subject, Department, Exam Date (`DD-MM-YYYY`), Start Time, End Time (e.g. `10:00 AM`)
- **Flow:** `Exam Details → Manage → Store → View/Search`

---

### 📋 6. Invigilator Allocation

Assign invigilators to exam halls and avoid scheduling conflicts — this is the "smart" part of the system.


- **Main class:** `Allocation`
- **Functions:** `allocateInvigilator()` · `viewAllocations()` · `checkConflict()` · `removeAllocation()`
- **Flow:** `Exam + Hall + Invigilator → Allocation → Conflict Check → Assignment`

**Conflict rules**

| Conflict | Result |
|---|---|
| Same invigilator, same date, overlapping time, different hall | ❌ Rejected — invigilator double-booked |
| Same hall and same exam already has an invigilator | ❌ Rejected — hall already allocated |
| No clash found | ✅ Allocation saved |

---

### 📊 7. Reports & Dashboard

Displays examination information, invigilator allocations and management summaries (total students, invigilators, halls, exams and allocations), with an **Export Report** option.


- **Main classes:** `Report` · `Dashboard`
- **Functions:** `generateReport()` · `viewReport()` · `showDashboard()`
- **Flow:** `Exam Data → Reports → Dashboard → Summary`

---

### 🧾 Sample Inputs & Outputs (Overview)

A single-page overview of the sample inputs and outputs across the whole system.

---

## 🎬 Project Demonstration Flow


1. **Login** — administrator logs in with valid credentials
2. **Dashboard** — overview of all modules with quick navigation
3. **Student Management** — add, view, update and delete student records
4. **Invigilator Management** — manage invigilator details and availability
5. **Exam Hall Management** — manage hall details and capacity
6. **Exam Management** — add exam schedule and subject details
7. **Invigilator Allocation** — assign invigilators and check for conflicts
8. **Reports & Dashboard** — view reports and management summaries

---

## 🧠 C++ OOP Concepts Used

| # | Concept | Where it is used |
|---|---|---|
| 1 | **Classes & Objects** | Organize the different project modules |
| 2 | **Encapsulation** | Data and functions bundled inside each class |
| 3 | **Member Functions** | Add, view, search, update and delete operations |
| 4 | **File Handling** | Storing and retrieving data from CSV files |
| 5 | **Conditional Statements** | Validation and decision-making |

```cpp
class Student {
public:
    void addStudent();
    void viewStudents();
};
```

---

## 📁 Project Structure

```
SmartExamInvigilation/
├── app.py                  # Launcher: compiles with MSVC (cl.exe) and runs the app
├── sfml_config.txt         # Path to your SFML folder
├── SETUP.md                # Detailed setup guide
├── assets/                 # Logos
├── src/
│   ├── main.cpp            # Entry point (SFML window + screen navigation)
│   └── core_test.cpp       # Console test harness for the core logic (not part of the GUI build)
├── include/
│   ├── Admin.h             # Login / password reset
│   ├── Student.h           # Student model + manager
│   ├── Invigilator.h       # Invigilator model + manager
│   ├── ExamHall.h          # Hall model + manager
│   ├── Exam.h              # Exam model + manager
│   ├── Allocation.h        # Allocation + conflict detection
│   ├── Report.h            # Summary + report export
│   ├── FileUtils.h         # CSV read/write helpers
│   └── gui/                # SFML screens (Login, Dashboard, Student, Invigilator,
│                           #   Hall, Exam, Allocation, Reports) + Widgets, Theme, Sidebar, HeaderBar
└── data/                   # CSV files, created automatically on first run
    ├── admin.csv
    ├── students.csv
    ├── invigilators.csv
    ├── halls.csv
    ├── exams.csv
    └── allocations.csv
```

---

## 🚀 Getting Started

### Prerequisites

- **Windows** with **Visual Studio** and the **"Desktop development with C++"** workload
- **SFML 2.6.1** (Visual C++ build matching your Visual Studio version — `vc17` for VS 2022, `vc16` for VS 2019)
- **Python 3** (only to run the `app.py` launcher)

### Steps

1. **Clone the repository**
   ```bash
   git clone https://github.com/<your-username>/<your-repo-name>.git
   cd <your-repo-name>
   ```

2. **Download SFML** from <https://www.sfml-dev.org/download/sfml/2.6.1/> and extract it, e.g. to `C:\SFML-2.6.1`

3. **Set the SFML path** — open `sfml_config.txt` and put your SFML folder path on a single line:
   ```
   C:\SFML-2.6.1
   ```

4. **Build and run**
   ```bash
   python app.py
   ```
   `app.py` compiles everything under `src/` (except `core_test.cpp`) with `cl.exe`, copies the SFML DLLs next to the executable and launches the app.

> 📘 For troubleshooting and more detail, see [SETUP.md](SETUP.md).

---

## 🔑 Default Login

| Username | Password |
|---|---|
| `Admin` | `Admin123` |

The credentials are stored in `data/admin.csv`, which is created on first run. You can change them there without recompiling.

---

## ✅ Testing

### Valid input testing

| Module | Valid Input | Expected Output |
|---|---|---|
| 🔐 Login | Correct username & password | Dashboard opens |
| 👨‍🎓 Student | Valid student details | Student record added |
| 👨‍🏫 Invigilator | Valid details | Invigilator record added |
| 🏫 Exam Hall | Valid hall details & capacity | Hall record added |
| 📅 Exam | Valid exam details | Exam record created |
| 📋 Allocation | Available invigilator | Allocation successful |

### Invalid input testing

| Test Case | Input Condition | Expected Result |
|---|---|---|
| 🔐 Login | Wrong password | Login rejected |
| 👨‍🎓 Student | Invalid / missing details | Validation required |
| 🏫 Exam Hall | Invalid capacity | Input rejected |
| 📋 Allocation | Unavailable invigilator | Allocation rejected |
| ⚠️ Allocation | Scheduling conflict | Conflict detected |
| 🔎 Search | Non-existing record | Record not found |

The core logic (models and conflict detection) can also be checked from the console using `src/core_test.cpp`.

---
