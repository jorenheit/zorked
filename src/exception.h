#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <sstream>
#include <filesystem>

namespace Exception {

  struct ExceptionBase {
    virtual std::string what() const {
      return "ERROR: " + (this->what());
    }
  };

  struct JSONFormatError  {
    std::string _trace;
    
    JSONFormatError(std::string const &trace):
      _trace(trace)
    {}
    
    virtual std::string what() const {
      std::ostringstream out;
      out << "ERROR: " << _trace << "'\n->  " << this->what();
      return out.str();
    }
  };
  
  struct ErrorOpeningFile: public ExceptionBase {
    std::string _filename;
    ErrorOpeningFile(std::string const &fname):
      _filename(fname)
    {}
    
    virtual std::string what() const override {
      return "Could not open file: \'" + _filename + "\'";
    }
  };

  struct UndefinedReference: public JSONFormatError {
    std::string _id;

    UndefinedReference(std::string const &trace, std::string const &id):
      JSONFormatError(trace),
      _id(id)
    {}
    
    virtual std::string what() const override {
      return "Undefined reference to ID '" + _id + "'.";
    }
  };

  struct MultipleDefinitions: public JSONFormatError {
    std::string _id;
    MultipleDefinitions(std::string const &trace, std::string const &id):
      JSONFormatError(trace),
      _id(id)
    {}
    
    virtual std::string what() const override {
      return "Multiple defefinitions of ID '" + _id + "'.";
    }
  };
  
  struct SpecificFormatError: public JSONFormatError {
    std::string msg;

    template <typename ... Args>
    SpecificFormatError(std::string const &trace, Args&& ... args):
      JSONFormatError(trace)
    {
      std::ostringstream out;
      (out << ... << std::forward<Args>(args));
      msg = out.str();
    }
    
    virtual std::string what() const override {
      return msg;
    }
  };

  struct ExpectedField: public JSONFormatError {
    std::string _key;
    
    ExpectedField(std::string const &trace, std::string const &key):
      JSONFormatError(trace),
      _key(key)
    {}
    
    virtual std::string what() const override {
      return "Expected key '" + _key + "'.";
    }
  };

  struct InvalidFieldType: public JSONFormatError {
    std::string _key, _expected, _supplied;

    InvalidFieldType(std::string const &trace, std::string const &key,
		     std::string const &expected, std::string const &supplied):
      JSONFormatError(trace),
      _key(key),
      _expected(expected),
      _supplied(supplied)
    {}

    virtual std::string what() const override {
      return "Expected type '" + _expected + "' but got '" + _supplied + "' in field '" + _key + "'.";
    }
  };

  struct DoubleConnectedLocation: public JSONFormatError {
    std::string _from, _to;
    
    DoubleConnectedLocation(std::string const &trace,std::string const &from, std::string const &to):
      JSONFormatError(trace),
      _from(from),
      _to(to)
    {}
    
    virtual std::string what() const override {
      return "Location '" + _from + "' was already connected to location '" + _to + "'.";
    }
  };

  struct InvalidSaveFile: ExceptionBase {
    virtual std::string what() const override {
      return "Save-file is not compatible with the story-files.";
    }
  };

} // namespace Exception


#endif //EXCEPTION_H
