#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <iomanip>
using namespace std;

class Section;
class Course;
class Student;
class Professor;
class TranscriptEntry;
class Schedule;

class Person
{
protected:
    string id;
    string name;

public:
    string getId() { return id; }
    void setId(string id) { this->id = id; }
    string getName() { return name; }
    void setName(string name) { this->name = name; }

    Person(string id, string name) : id(id), name(name) {}

    virtual void display() {
        cout << "+ \tId: " << id << endl;
        cout << "+ \tName: " << name << endl;
    }
};

class Schedule {
private:
    map<string, map<string, vector<Section*>>> schedule;
    const vector<string> days = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};
    const vector<string> timeSlots = {"Morning", "Afternoon", "Evening"};

    string getTimeSlot(const string& timeOfDay) {
        if (timeOfDay.find("AM") != string::npos) return "Morning";
        else if (timeOfDay.find("PM") != string::npos && timeOfDay.substr(0, 2) < "05") return "Afternoon";
        else return "Evening";
    }

public:
    void addSection(Section* section);

    void display();
};

class Course
{
private:
    string courseNo;
    string courseName;
    int credits;
    map<string, Course*> prerequisites;
    map<string, Section*> sections;
    Schedule& schedule;

public:
    string getCourseNo() { return courseNo; }
    void setCourseNo(string courseNo) { this->courseNo = courseNo; }
    string getCourseName() { return courseName; }
    void setCourseName(string courseName) { this->courseName = courseName; }
    int getCredits() { return credits; }
    void setCredits(int credits) { this->credits = credits; }

    Course(string courseNo, string courseName, int credits, Schedule& schedule)
        : courseNo(courseNo), courseName(courseName), credits(credits), schedule(schedule) {}

    void addPrerequisite(Course* course) {
        prerequisites[course->getCourseNo()] = course;
    }

    map<string, Course*>& getPrerequisites() { return prerequisites; }

    void display() {
        cout << "+ \tCourse number: " << courseNo << endl;
        cout << "+ \tCourse name: " << courseName << endl;
        cout << "+ \tCredits: " << credits << endl;
        if (!prerequisites.empty()) {
            cout << "Prerequisites: " << endl;
            for (const auto& p : prerequisites) {
                p.second->display();
            }
        } else {
            cout << "No prerequisites." << endl;
        }
    }

    void scheduleSections(string sectionNo, string dayOfWeek, string timeOfDay, string semester, string room, int seatingCapacity);
    Section* findASection(string sectionName);
};

class Section : public Course
{
private:
    Professor* teacher;
    string sectionNo;
    string dayOfWeek;
    string timeOfDay;
    string semester;
    string room;
    int seatingCapacity;
    map<string, Student*> enrolledStudents;
    map<string, TranscriptEntry*> grades;

public:
    string getSectionNo() { return sectionNo; }
    void setSectionNo(string sectionNo) { this->sectionNo = sectionNo; }
    string getDayOfWeek() { return dayOfWeek; }
    void setDayOfWeek(string dayOfWeek) { this->dayOfWeek = dayOfWeek; }
    string getTimeOfDay() { return timeOfDay; }
    void setTimeOfDay(string timeOfDay) { this->timeOfDay = timeOfDay; }
    string getSemester() { return semester; }
    void setSemester(string semester) { this->semester = semester; }
    string getRoom() { return room; }
    void setRoom(string room) { this->room = room; }
    int getSeatingCapacity() { return seatingCapacity; }
    void setSeatingCapacity(int seatingCapacity) { this->seatingCapacity = seatingCapacity; }

    Section(string courseNo, string courseName, int credits, string sectionNo, string dayOfWeek, string timeOfDay, string semester, string room, int seatingCapacity, Schedule& schedule)
        : Course(courseNo, courseName, credits, schedule), sectionNo(sectionNo), dayOfWeek(dayOfWeek), timeOfDay(timeOfDay), semester(semester), room(room), seatingCapacity(seatingCapacity), teacher(nullptr) {
        schedule.addSection(this);
    }

    void display();

    void enrollStudent(Student* student);
    void postGrade(Student* student, int grade);
    friend class Professor;
};

class Professor : public Person
{
private:
    string title;
    string department;
    map<string, Section*> sections;

public:
    string getTitle() { return title; }
    void setTitle(string title) { this->title = title; }
    string getDepartment() { return department; }
    void setDepartment(string department) { this->department = department; }

    Professor(string id, string name, string title, string department)
        : Person(id, name), title(title), department(department) {}

    void display() override {
        cout << "Professor:" << endl;
        Person::display();
        cout << "+ \tTitle: " << title << endl;
        cout << "+ \tDepartment: " << department << endl;
    }

    void agreeToTeach(Section* section);
};

class TranscriptEntry
{
private:
    Section* section;
    Student* student;
    int grade;

public:
    Student* getStudent() { return student; }
    void setStudent(Student* student) { this->student = student; }
    Section* getSection() { return section; }
    void setSection(Section* section) { this->section = section; }
    int getGrade() { return grade; }
    void setGrade(int grade) { this->grade = grade; }

    TranscriptEntry(Section* section, Student* student, int grade)
        : section(section), student(student), grade(grade) {}

    void display() {
        cout << "Course: " << section->getCourseName() << " - Grade: " << grade << endl;
    }
};

class Transcript
{
private:
    Student* student;
    map<string, TranscriptEntry*> transcriptEntries;

public:
    Transcript(Student* student) : student(student) {}

    void addEntry(TranscriptEntry* entry) {
        transcriptEntries[entry->getSection()->getCourseNo()] = entry;
    }

    const map<string, TranscriptEntry*>& getTranscriptEntries() const {
        return transcriptEntries;
    }

    void display();

    ~Transcript() {
        for (auto& entry : transcriptEntries) {
            delete entry.second;
        }
    }
};

class Student : public Person
{
private:
    string major;
    string degree;
    Professor* advisor;
    map<string, Course*> courses;

public:
    string getMajor() { return major; }
    void setMajor(string major) { this->major = major; }
    string getDegree() { return degree; }
    void setDegree(string degree) { this->degree = degree; }
    Professor* getAdvisor() { return advisor; }
    void setAdvisor(Professor* advisor) { this->advisor = advisor; }

    Student(string id, string name, string major, string degree)
        : Person(id, name), major(major), degree(degree), advisor(nullptr) {
        transcript = new Transcript(this);
    }

    void addCourse(Course* course) {
    bool canAdd = true;
    for (const auto& prereq : course->getPrerequisites()) {
        auto transcriptEntry = transcript->getTranscriptEntries().find(prereq.first);
        if (transcriptEntry == transcript->getTranscriptEntries().end() || transcriptEntry->second->getGrade() <= 5) {
            canAdd = false;
            cout << "Prerequisite " << prereq.first << " not met or grade not above 5." << endl;
            break;
        }
    }

    if (canAdd) {
        courses[course->getCourseNo()] = course;
        cout << "Added " << course->getCourseName() << " to plan of study." << endl;
    } else {
        cout << "Cannot add " << course->getCourseName() << " because prerequisites are not met." << endl;
    }
}

    void display() override {
        cout << "Student:" << endl;
        Person::display();
        cout << "+ \tMajor: " << major << endl;
        cout << "+ \tDegree: " << degree << endl;
        if (advisor) {
            cout << "Advisor: " << advisor->getName() << endl;
        } else {
            cout << "No advisor assigned yet." << endl;
        }
    }

    void displayPlanOfStudy() {
        cout << name << "'s plan of study" << endl;
        if (!courses.empty()) {
            for (const auto& course : courses) {
                course.second->display();
            }
        } else {
            cout << "No courses in the plan of study." << endl;
        }
    }

    Transcript* transcript;

    ~Student() {
        delete transcript;
    }
};

class GPA
{
private:
    Transcript* transcript;
    double gpa;

public:
    GPA(Transcript* transcript) : transcript(transcript) {
        calculateGPA();
    }

    void calculateGPA() {
        double totalCredits = 0;
        double totalGradePoints = 0;

        for (const auto& entry : transcript->getTranscriptEntries()) {
            Section* section = entry.second->getSection();
            int credits = section->getCredits();
            int grade = entry.second->getGrade();

            totalCredits += credits;
            totalGradePoints += grade * credits;
        }

        if (totalCredits > 0) {
            gpa = totalGradePoints / (totalCredits * 25); // Assuming grade is out of 100, divide by 25 to get 4.0 scale
        } else {
            gpa = 0.0;
        }
    }

    double getGPA() { return gpa; }

    void display() {
        cout << "GPA: " << fixed << setprecision(2) << gpa << endl;
    }
};

void Schedule::addSection(Section* section) {
    string day = section->getDayOfWeek().substr(0, 3);
    string timeSlot = getTimeSlot(section->getTimeOfDay());

    // Check for conflicts
        if (!schedule[day][timeSlot].empty()) {
            cout << "Warning: Scheduling conflict detected!" << endl;
            cout << "Existing course(s) in the same time slot:" << endl;
            for (const auto& existingSection : schedule[day][timeSlot]) {
                cout << "- " << existingSection->getCourseNo() << " " << existingSection->getSectionNo() << endl;
            }
            cout << "New course being added: " << section->getCourseNo() << " " << section->getSectionNo() << endl;
        }

    schedule[day][timeSlot].push_back(section);
}

void Course::scheduleSections(string sectionNo, string dayOfWeek, string timeOfDay, string semester, string room, int seatingCapacity) {
    Section* aSection = new Section(courseNo, courseName, credits, sectionNo, dayOfWeek, timeOfDay, semester, room, seatingCapacity, schedule);
    sections[sectionNo] = aSection;
}

Section* Course::findASection(string sectionName) {
    auto it = sections.find(sectionName);
    if (it != sections.end()) {
        return it->second;
    } else {
        cout << "Can't find section" << endl;
        return nullptr;
    }
}

void Section::enrollStudent(Student* student) {
    if (enrolledStudents.size() < seatingCapacity && teacher != nullptr) {
        // Check prerequisites
        bool canEnroll = true;
        for (const auto& prereq : getPrerequisites()) {
            auto transcriptEntry = student->transcript->getTranscriptEntries().find(prereq.first);
            if (transcriptEntry == student->transcript->getTranscriptEntries().end() || transcriptEntry->second->getGrade() <= 5) {
                canEnroll = false;
                cout << "Prerequisite " << prereq.first << " not met or grade not above 5 for " << student->getName() << "." << endl;
                break;
            }
        }

        if (canEnroll) {
            enrolledStudents[student->getId()] = student;
            cout << student->getName() << " enrolled in section " << sectionNo << " successfully." << endl;
        } else {
            cout << "Cannot enroll " << student->getName() << " in section " << sectionNo << " due to unmet prerequisites." << endl;
        }
    } else if (enrolledStudents.size() >= seatingCapacity) {
        cout << "Section " << sectionNo << " is full." << endl;
    } else {
        cout << "Section " << sectionNo << " has no teacher." << endl;
    }
}

void Section::postGrade(Student* student, int grade) {
    auto it = enrolledStudents.find(student->getId());
    if (it != enrolledStudents.end()) {
        TranscriptEntry* entry = new TranscriptEntry(this, student, grade);
        grades[student->getId()] = entry;
        student->transcript->addEntry(entry);
        cout << "Grade posted for " << student->getName() << " in " << getCourseName() << ": " << grade << endl;
    } else {
        cout << "Error: " << student->getName() << " is not enrolled in this section." << endl;
    }
}

void Section::display() {
    Course::display();
        cout << "+ \tSection number: " << sectionNo << endl;
        cout << "+ \tDay of week: " << dayOfWeek << endl;
        cout << "+ \tTime of day: " << timeOfDay << endl;
        cout << "+ \tSemester: " << semester << endl;
        cout << "+ \tRoom: " << room << endl;
        cout << "+ \tSeating Capacity: " << seatingCapacity << endl;
        if (teacher) {
            cout << "+ \tTaught by: " << teacher->getName() << endl;
        } else {
            cout << "+ \tNo teacher assigned yet." << endl;
        }
}

void Professor::agreeToTeach(Section* section) {
    if (section->teacher == nullptr) {
        section->teacher = this;
        sections[section->getSectionNo()] = section;
        cout << "Professor " << getName() << " has agreed to teach " << section->getCourseName() << " section " << section->getSectionNo() << endl;
    } else {
        cout << "Error: Section " << section->getSectionNo() << " already has a teacher assigned." << endl;
    }
}

void Schedule::display() {
        cout << setw(10) << "Time" << "|";
        for (const auto& day : days) {
            cout << setw(15) << day << "|";
        }
        cout << endl;
        cout << string(120, '-') << endl;

        for (const auto& timeSlot : timeSlots) {
            cout << setw(10) << timeSlot << "|";
            for (const auto& day : days) {
                string cellContent;
                if (schedule[day].count(timeSlot) > 0) {
                    for (const auto& section : schedule[day][timeSlot]) {
                        cellContent += section->getCourseNo() + " " + section->getSectionNo() + "\n";
                    }
                }
                cout << setw(15) << cellContent << "|";
            }
            cout << endl;
            cout << string(120, '-') << endl;
        }
    }

void Transcript::display() {
    cout << "Transcript for " << student->getName() << ":" << endl;
    for (const auto& entry : transcriptEntries) {
        entry.second->display();
    }
    
    GPA gpa(this);
    gpa.display();
}

int main() {
    Schedule schedule;

    // Create some students
    Student* s1 = new Student("1", "Alice", "Computer Science", "BSc");
    Student* s2 = new Student("2", "Bob", "Mathematics", "BSc");

    // Create some professors
    Professor* p1 = new Professor("P1", "Dr. Smith", "Associate Professor", "Computer Science");

    // Set advisors
    s1->setAdvisor(p1);

    // Create some courses
    Course* c1 = new Course("CS101", "Introduction to Computer Science", 4, schedule);
    Course* c2 = new Course("CS102", "Data Structures", 4, schedule);
    Course* c3 = new Course("CS201", "Algorithms", 4, schedule);
    Course* c4 = new Course("MATH101", "Calculus", 4, schedule);

    // Add prerequisites to courses
    c3->addPrerequisite(c1);
    c3->addPrerequisite(c2);

    // Add courses to the plans of study
    s1->addCourse(c1);
    s1->addCourse(c2);
    s1->displayPlanOfStudy();

    // Create some sections
    c1->scheduleSections("S1", "Monday", "9:00 AM", "Fall 2023", "Room A", 30);
    c2->scheduleSections("S2", "Wednesday", "11:00 AM", "Fall 2023", "Room B", 25);
    c3->scheduleSections("S3", "Monday", "9:00 AM", "Fall 2023", "Room C", 20);

    Section* sec1 = c1->findASection("S1");
    Section* sec2 = c2->findASection("S2");
    Section* sec3 = c3->findASection("S3");

    if (sec1 && sec2 && sec3) {
        // Assign a professor to teach the sections
        p1->agreeToTeach(sec1);
        p1->agreeToTeach(sec2);
        p1->agreeToTeach(sec3);

        // Try to enroll student in sections
        sec1->enrollStudent(s1);
        
        // Post grade for CS101
        sec1->postGrade(s1, 95);

        // Try to add CS102 to plan of study
        s1->addCourse(c2);

        // Try to enroll in CS102
        sec2->enrollStudent(s1);

        // Try to add CS201 to plan of study (should fail due to CS102 prerequisite)
        s1->addCourse(c3);

        // Display the transcript
        s1->transcript->display();

        // Display the schedule
        schedule.display();
    }
    
    // Clean up
    delete s1;
    delete s2;
    delete p1;
    delete c1;
    delete c2;
    delete c3;

    return 0;
}