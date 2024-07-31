#include <exception>

namespace jal
{
    using namespace std;

    enum class ErrorLevel: int
    {
        io=0,
        document=1,
        objectModel=2
    };

    class JalException : public exception
    {
    public:
        virtual const char* what() const noexcept override;
    };
}

