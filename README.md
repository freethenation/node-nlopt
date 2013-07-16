# About #
NLopt is a free/open-source library for nonlinear optimization, providing a common interface for a number of different free optimization routines available online as well as original implementations of various other algorithms. Node-nlopt is a JS wrapper around nlopt. For reference about the different algorithms available and the parameters they accept you should consult [NLopt's website](http://ab-initio.mit.edu/wiki/index.php/NLopt).

# Installation #
Run the command

`npm install nlopt`

Running this command builds nlopt which is a c library. I have tested that it build on Windows 64bit and Linux 64bit. I have not tested other platforms

# Simple Example #
The library defines a single method. A simple example of how to use the library can be found below. This is the same example used in the [tutorial at NLopt's website](http://ab-initio.mit.edu/wiki/index.php/NLopt_Tutorial).

```javascript
var nlopt = requires('nlopt');
var myfunc = function(n, x, grad){
  if(grad){
    grad[0] = 0.0;
    grad[1] = 0.5 / Math.sqrt(x[1]);
  }
  return Math.sqrt(x[1]);
}
var createMyConstraint = function(cd){
  return {
    callback:function(n, x, grad){
      if(grad){
        grad[0] = 3.0 * cd[0] * (cd[0]*x[0] + cd[1]) * (cd[0]*x[0] + cd[1])
        grad[1] = -1.0
      }
      tmp = cd[0]*x[0] + cd[1]
      return tmp * tmp * tmp - x[1]
    },
    tolerance:1e-8
  }
}
options = {
  algorithm: "LD_MMA",
  numberOfParameters:2,
  minObjectiveFunction: myfunc,
  inequalityConstraints:[createMyConstraint([2.0, 0.0]), createMyConstraint([-1.0, 1.0])],
  xToleranceRelative:1e-4,
  initalGuess:[1.234, 5.678],
  lowerBounds:[Number.MIN_VALUE, 0]
}
console.log(nlopt(options).parameterValues);
```
The code above should write "[ 0.33333333465873644, 0.2962962893886998 ]" to the console.

# API #

The library defines a single function that takes a JavaScript object as a parameter. The format for the JavaScript is:
```javascript
{
	//The algorithm to run. Look at the nlopt site for a complete list of options
	algorithm: "LD_MMA",
	//The number of parameters that the function to be optimized takes
    numberOfParameters:2,
    //The function to be minified.
    minObjectiveFunction: function(numberOfParameters, parameterValues, gradient){},
    //The function to be maximized. If minObjectiveFunction is specified this option should not be.
    maxObjectiveFunction: function(numberOfParameters, parameterValues, gradient){},
    //An inital guess of the values that maximize or minimize the objective function
    initalGuess:[1.234, 5.678],
    //Parameter values must be above the provided numbers
    lowerBounds:[Number.MIN_VALUE, 0],
    //Parameter values must be below the provided numbers
    upperBounds:[Number.MIN_VALUE, 0],
    //Inequality constraints on the function to be optimized.
    inequalityConstraints:[function(numberOfParameters, parameterValues, gradient), function(){}],
    //Equalit constraints on the function to be optimized.
    equalityConstraints:[function(numberOfParameters, parameterValues, gradient), function(){}],
    //Consult http://ab-initio.mit.edu/wiki/index.php/NLopt_Reference#Stopping_criteria for more info
    //on the next couple of options
  	stopValue: 1e-4,
  	fToleranceRelative: 1e-4,
  	fToleranceAbsolute: 1e-4,
  	xToleranceRelative: 1e-4,
  	xToleranceAbsolute: 1e-4,
  	maxEval: 1e-4,
  	maxTime: 1e-4,
}
```
The return value has the format
```javascript
{
	//The parameter values that produce the min or max value for the object function
	parameterValues: [ 0.33333333465873644, 0.2962962893886998 ],
	//The min or max function for the objective function.
   	outputValue: 0.5443310476067847 ,
   	//A string indicating if optimization was successful. If optimization was successful the string will
   	//start with "Success"
   	status: 'Success: Optimization stopped because xToleranceRelative or xToleranceAbsolute was reached',
   	//A string will also be outputed for each setting/option set. This string will also start with "Success"
   	//if the operation was successful. Examples can be found below.
   	maxObjectiveFunction: 'Success',
    lowerBounds: 'Success'
}
```
Some of the descriptions above are incomplete. Consult [NLopt's website](http://ab-initio.mit.edu/wiki/index.php/NLopt) for more info on the various options.

# Limitations #
The biggest limitation at the moment is there is currently no support for making the call to nlopt asynchronously. Numerical optimization is inherently CPU bound so asynchronously calling nlopt is not incredibly useful and I did not need it for my use case.