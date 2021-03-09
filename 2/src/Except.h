#ifndef __EXCEPT_H__
#define __EXCEPT_H__

#include <iostream>
#include <string>
#include <exception>
using namespace std;

class StackException : public exception
{
public:
    virtual const char* what() const throw()
    {
        return "Stack error!";
    }
};

class OperandException : public exception
{
public:
    virtual const char* what() const throw()
    {
        return "Operand error!";
    }
};

class OpcodeException : public exception
{
public:
    virtual const char* what() const throw()
    {
        return "Opcode error!";
    }
};

class InstrErrorException : public exception
{
public:
    virtual const char* what() const throw()
    {
        return "Instruction error!";
    }
};

#endif