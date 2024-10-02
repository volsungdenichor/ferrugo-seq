#include <ferrugo/core/demangle.hpp>
#include <ferrugo/core/ostream_utils.hpp>
#include <ferrugo/core/pipe.hpp>
#include <ferrugo/core/std_ostream.hpp>
#include <ferrugo/seq/pipe.hpp>
#include <ferrugo/seq/seq.hpp>
#include <functional>
#include <iomanip>
#include <iostream>
#include <limits>
#include <optional>
#include <string>

#define L(...) [](auto&& it) -> decltype((__VA_ARGS__)) { return (__VA_ARGS__); }

struct Student
{
    int id;
    std::string name;
};

struct Classroom
{
    int id;
    std::string name;
};

struct Student_Classroom
{
    int student_id;
    int classroom_id;
};

int main()
{
    using namespace ferrugo;
    using namespace ferrugo::seq;

    const std::vector<Student> students = { { 1, "Adam" }, { 2, "Beata" }, { 3, "Celina" }, { 4, "Dorota" } };
    const std::vector<Classroom> classrooms = { { 10, "Biologia" }, { 11, "Geografia" }, { 12, "ZPT" } };
    const std::vector<Student_Classroom> student_classroom = { { 1, 10 }, { 2, 10 }, { 3, 11 }, { 4, 11 }, { 1, 12 } };

    cartesian_product(view(students), view(classrooms), view(student_classroom))  //
        |= seq::filter(
            [](const Student& student, const Classroom& classroom, const Student_Classroom& student_classroom)
            { return student_classroom.classroom_id == classroom.id && student_classroom.student_id == student.id; })
        |= seq::for_each([](const Student& student, const Classroom& classroom, const Student_Classroom& student_classroom)
                         { std::cout << student.name << " " << classroom.name << "\n"; });

    // const std::vector<std::string> values = range('A', 'E') |= transform(core::str);

    // for (const auto&& [a, b, c, d] : cartesian_product(range(2), view(values), range(3), range(4)))
    // {
    //     std::cout << a << " " << b << " " << c << " " << d << "\n";
    // }
}
