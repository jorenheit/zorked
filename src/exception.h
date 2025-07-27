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
    std::string _filename;
    std::string _trace;
    
    JSONFormatError(std::filesystem::path const &path, std::string const &trace):
      _filename(path.filename()),
      _trace(trace)
    {}
    
    virtual std::string what() const {
      std::ostringstream out;
      out << "ERROR: in file '" << _filename << "', object '" << _trace << "'\n->  " << this->what();
      return out.str();
    }
  };
  
  struct ErrorOpeningFile: public ExceptionBase {
    std::string msg;
    ErrorOpeningFile(std::string const &filename) {
      msg = "Could not open file: \'" + filename + "\'";
    }
    
    virtual std::string what() const override {
      return msg;
    }
  };

  struct UndefinedReference: public JSONFormatError {
    std::string msg;
    UndefinedReference(std::string const &path, std::string const &trace, std::string const &id):
      JSONFormatError(path, trace)
    {
      msg = "Undefined reference to ID '" + id + "'.";
    }
    
    virtual std::string what() const override {
      return msg;
    }
  };

  struct SpecificFormatError: public JSONFormatError {
    std::string msg;

    template <typename ... Args>
    SpecificFormatError(std::string const &path, std::string const &trace, Args&& ... args):
      JSONFormatError(path, trace)
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
    std::string msg;
    
    ExpectedField(std::string const &path, std::string const &trace, std::string const &key):
      JSONFormatError(path, trace)
    {
      msg = "Expected key '" + key + "'.";
    }

    virtual std::string what() const override {
      return msg;
    }
  };

  struct InvalidFieldType: public JSONFormatError {
    std::string msg;

    InvalidFieldType(std::string const &path, std::string const &trace, std::string const &key,
		     std::string const &expected, std::string const &supplied):
      JSONFormatError(path, trace)
    {
      msg = "Expected type '" + expected + "' but got '" + supplied + "' in field '" + key + "'.";
    }

    virtual std::string what() const override {
      return msg;
    }
  };

  struct DoubleConnectedLocation: public JSONFormatError {
    std::string msg;
    
    DoubleConnectedLocation(std::string const &path, std::string const &trace,
			    std::string const &from, std::string const &to):
      JSONFormatError(path, trace)
    {
      msg = "Location '" + from + "' was already connected to location '" + to + "'.";
    }

    virtual std::string what() const override {
      return msg;
    }
  };

} // namespace Exception


#endif //EXCEPTION_H
