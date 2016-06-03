#ifndef CONSTRAINT_HPP
#define CONSTRAINT_HPP


#include <eoFunctor.h>
#include "chromosome/eoChromosome.h"


/**
    Basic Constant Unary Functor. Derive from this class when defining
    any unary function. First template argument is the first_argument_type,
    second result_type.
    Argument and result types can be any type including void for
    result_type
**/
template <class A1, class R>
class eoConstUF : public eoFunctorBase, public std::unary_function<A1, R>
{
public :

    /// virtual dtor here so there is no need to define it in derived classes
    virtual ~eoConstUF() {}

    /// The pure virtual function that needs to be implemented by the subclass
    virtual R operator()(A1) const = 0;

    /// tag to identify a procedure in compile time function selection @see functor_category
    static eoFunctorBase::unary_function_tag functor_category()
    {
        return eoFunctorBase::unary_function_tag();
    }
};





// A constraint (hard or soft) is a unary function that receives a eoChromosome
// and returns a double value representing the penalty incurred (in the case of a soft constraint)
// or the distance to feasibilty (in the case of a hard constraint)

//template <typename EOT>
//class Constraint : public eoConstUF<EOT, double> { };

class Constraint : public eoConstUF<eoChromosome const&, double> { };


class BinaryConstraint : public Constraint {

protected:
    // Protected constructor
    BinaryConstraint(int _e1, int _e2)
        : e1(_e1), e2(_e2) { }

public:
    int getE1() const { return e1; }
    int getE2() const { return e2; }

private:
    int e1, e2; // Exams indexes
};


class UnaryConstraint : public Constraint {

protected:
    // Protected constructor
    UnaryConstraint(int _e)
        : e(_e) { }

public:
    int getE() const { return e; }

private:
    int e; // Exam index
};


#endif // CONSTRAINT_HPP
