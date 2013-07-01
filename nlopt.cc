#include <node.h>
#include <v8.h>
#include <math.h>
#include <nlopt.h>

using namespace v8;


Local<Array> cArrayToV8Array(unsigned n, const double* array)
{
  Local<Array> ret = Local<Array>::New(Array::New(n));
  for (unsigned i = 0; i < n; ++i)
  {
    ret->Set(i, Number::New(array[i]));
  }
  return ret;
}

double* v8ArrayToCArray(Local<Array>& array)
{
  double* ret = new double[array->Length()];
  for (unsigned i = 0; i < array->Length(); ++i)
  {
    ret[i] = array->Get(i)->NumberValue();
  }
  return ret;
}

void checkNloptErrorCode(Local<Object>& errors, Local<String>& operation, nlopt_result errorCode)
{
  const char* str;
  switch (errorCode)
  {
    case NLOPT_SUCCESS: 
      str = "Success";
      break;
    case NLOPT_STOPVAL_REACHED:
      str = "Optimization stopped because stopval was reached";
      break;
    case NLOPT_FTOL_REACHED:
      str = "Optimization stopped because ftol_rel or ftol_abs was reached";
      break;
    case NLOPT_XTOL_REACHED:
      str = "Optimization stopped because xtol_rel or xtol_abs was reached";
      break;
    case NLOPT_MAXEVAL_REACHED:
      str = "Optimization stopped because maxeval was reached";
      break;
    case NLOPT_MAXTIME_REACHED:
      str = "Optimization stopped because maxtime was reached";
      break;
    case NLOPT_FAILURE:
      str = "Failer";
      break;
    case NLOPT_INVALID_ARGS:
      str = "Invalid arguments";
      break;
    case NLOPT_OUT_OF_MEMORY:
      str = "Ran out of memory";
      break;
    case NLOPT_ROUNDOFF_LIMITED:
      str = "Halted because roundoff errors limited progress";
      break;
    case NLOPT_FORCED_STOP:
      str = "Halted because of a forced termination";
      break;
    default:
      str = "Unknown Error Code";
      break;
  }
  //set error message for operation
  errors->Set(operation, String::New(str));
}

#define GET_VALUE(TYPE, NAME, OBJ) \
  Local<String> key_##NAME = Local<String>::New(String::NewSymbol(#NAME)); \
  Local<TYPE> val_##NAME; \
  if(OBJ->Has(key_##NAME)){ \
    val_##NAME = Local<TYPE>::Cast(OBJ->Get(key_##NAME)); \
  }

#define CHECK_CODE(NAME) checkNloptErrorCode(ret, key_##NAME, code);

#define SIMPLE_CONFIG_OPTION(NAME, CONFIG_METHOD) \
  GET_VALUE(Number, NAME, options) \
  if(!val_##NAME->IsUndefined()){ \
    code = CONFIG_METHOD(opt, val_##NAME->Value()); \
    CHECK_CODE(NAME) \
  }

double optimizationFunc(unsigned n, const double* x, double* grad, void* ptrCallback)
{
  HandleScope scope;
  Local<Value> undefined;
  Local<Function>* callback = (Local<Function>*)ptrCallback;
  double returnValue = -1;

  //prepare parms to callback
  Local<Value> argv[3];
  argv[0] = Local<Value>::New(Number::New(n));
  argv[1] = cArrayToV8Array(n, x);
  //gradient
  Local<Array> v8Grad;
  if(grad != NULL){
    v8Grad = cArrayToV8Array(n, grad);
  }
  argv[2] = v8Grad;
  //call callback
  Local<Value> ret = (*callback)->Call(Context::GetCurrent()->Global(), 3, argv);
  //validate return results
  if(!ret->IsNumber()){
    ThrowException(Exception::TypeError(String::New("Objective or constraint function must return a number.")));
  }
  else if(!v8Grad->IsUndefined() && v8Grad->Length() != n){
    ThrowException(Exception::TypeError(String::New("Length of gradient array must be the same as the number of parameters.")));
  }
  else { //success
    if(!v8Grad->IsUndefined()){
      for (unsigned i = 0; i < n; ++i) {
        grad[i] = v8Grad->Get(i)->NumberValue();
      }
    }
    returnValue = ret->NumberValue();
  }
  scope.Close(undefined);
  return returnValue;
}

Handle<Value> Optimize(const Arguments& args) {
  HandleScope scope;
  Local<Object> ret = Local<Object>::New(Object::New());
  nlopt_result code;
  Local<String> key;

  //There is not much validation in this function... should be done in js.
  Local<Object> options = Local<Object>::Cast(args[0]);

  //basic nlopt config
  GET_VALUE(Number, algorithm, options)
  if(val_algorithm->IsUndefined()){
    ThrowException(Exception::TypeError(String::New("algorithm must be specified")));
    return scope.Close(ret);
  }
  GET_VALUE(Number, numberOfParameters, options)
  if(val_numberOfParameters->IsUndefined()) {
    ThrowException(Exception::TypeError(String::New("numberOfParameters must be specified")));
    return scope.Close(ret);
  }
  unsigned n = val_numberOfParameters->Uint32Value();
  nlopt_opt opt;
  opt = nlopt_create((nlopt_algorithm)val_algorithm->Uint32Value(), n);

  //objective function
  GET_VALUE(Function, minObjectiveFunction, options)
  GET_VALUE(Function, maxObjectiveFunction, options)
  if(!val_minObjectiveFunction->IsUndefined()){
    code = nlopt_set_min_objective(opt, optimizationFunc, &val_minObjectiveFunction);
    CHECK_CODE(minObjectiveFunction)
  }
  else if(!val_maxObjectiveFunction->IsUndefined()){
    code = nlopt_set_max_objective(opt, optimizationFunc, &val_maxObjectiveFunction);
    CHECK_CODE(maxObjectiveFunction)
  }
  else{
    ThrowException(Exception::TypeError(String::New("minObjectiveFunction or maxObjectiveFunction must be specified")));
    return scope.Close(ret);
  }

  //optional parms
  GET_VALUE(Array, lowerBounds, options)
  if(!val_lowerBounds->IsUndefined()){
    double* lowerBounds = v8ArrayToCArray(val_lowerBounds);
    code = nlopt_set_lower_bounds(opt, lowerBounds);
    CHECK_CODE(lowerBounds)
    delete[] lowerBounds;
  }

  GET_VALUE(Array, upperBounds, options)
  if(!val_lowerBounds->IsUndefined()){
    double* upperBounds = v8ArrayToCArray(val_lowerBounds);
    code = nlopt_set_upper_bounds(opt, upperBounds);
    CHECK_CODE(upperBounds)
    delete[] upperBounds;
  }

  GET_VALUE(Array, inequalityConstraints, options)
  if(!val_inequalityConstraints->IsUndefined()){
    for (unsigned i = 0; i < val_inequalityConstraints->Length(); ++i)
    {
      Local<Object> obj = Local<Object>::Cast(val_inequalityConstraints->Get(i));
      GET_VALUE(Function, callback, obj)
      GET_VALUE(Number, tolerance, obj)
      code = nlopt_add_inequality_constraint(opt, optimizationFunc, &val_callback, val_tolerance->NumberValue());
      CHECK_CODE(inequalityConstraints)
    }
  }

  GET_VALUE(Array, equalityConstraints, options)
  if(!val_inequalityConstraints->IsUndefined()){
    for (unsigned i = 0; i < val_inequalityConstraints->Length(); ++i)
    {
      Local<Object> obj = Local<Object>::Cast(val_equalityConstraints->Get(i));
      GET_VALUE(Function, callback, obj)
      GET_VALUE(Number, tolerance, obj)
      code = nlopt_add_inequality_constraint(opt, optimizationFunc, &val_callback, val_tolerance->NumberValue());
      CHECK_CODE(equalityConstraints)
    }
  }

  SIMPLE_CONFIG_OPTION(stopValue, nlopt_set_stopval)
  SIMPLE_CONFIG_OPTION(stopBasedOnObjectiveFunction, nlopt_set_ftol_rel)
  SIMPLE_CONFIG_OPTION(stopBasedOnObjectiveFunctionAbs, nlopt_set_ftol_abs)
  SIMPLE_CONFIG_OPTION(stopBasedOnParameterChange, nlopt_set_xtol_rel)
  SIMPLE_CONFIG_OPTION(stopBasedOnParameterChangeAbs, nlopt_set_xtol_abs1)
  SIMPLE_CONFIG_OPTION(stopBasedOnMaxEvals, nlopt_set_maxeval)
  SIMPLE_CONFIG_OPTION(stopBasedOnMaxTime, nlopt_set_maxtime)

  //setup parms for optimization
  double input[n];
  double output[n];
  for (unsigned i = 0; i < n; ++i) {
    input[i] = 0;
    output[i] = 0;
  }
  //inital guess
  GET_VALUE(Array, intalGuess, options)
  if(!val_intalGuess->IsUndefined()){
    for (unsigned i = 0; i < n; ++i) {
      input[i] = val_intalGuess->Get(i)->NumberValue();
    }
  }
  //do the optimization!
  key = Local<String>::New(String::NewSymbol("optimzationResult"));
  checkNloptErrorCode(ret, key, nlopt_optimize(opt, input, output));
  ret->Set(String::NewSymbol("outValues"), cArrayToV8Array(n, input));
  ret->Set(String::NewSymbol("outObjectiveValues"), cArrayToV8Array(n, output));

  return scope.Close(ret);
}

void init(Handle<Object> exports) {
  exports->Set(String::NewSymbol("optimize"),
      FunctionTemplate::New(Optimize)->GetFunction());
}

NODE_MODULE(nlopt, init)
