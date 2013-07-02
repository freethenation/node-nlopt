_ = require('lodash')
expect = require('expect.js')
nlopt = require("../nlopt.js")
request = require("request")

describe('basic', ()->
  checkResults = (actual, expected)->
    equal = _.isEqual(actual, expected, (a,b)->
      if _.isNumber(a) and _.isNumber(b)
        expect(a).to.be.within(b-.01,b+.01)
        return true
      return undefined
    )
    if !equal then throw "#{if actual then JSON.stringify(actual) else 'undefined' } does not equal #{if expected then JSON.stringify(expected) else 'undefined'}"

  it('newton + simplex', ()->
    objectiveFunc = (n, x, grad)->
      if(grad)
        grad[0] = 16*x[0] - 4*x[0]*x[0]*x[0]
      x = 4 + 8*x[0]*x[0] - x[0]*x[0]*x[0]*x[0]

    #example from
    #http://www-rohan.sdsu.edu/~jmahaffy/courses/f00/math122/lectures/newtons_method/newtonmethodeg.html
    expectedResult = { 
      maxObjectiveFunction: 'Success'
      lowerBounds: 'Success'
      upperBounds: 'Success'
      xToleranceRelative: 'Success'
      inequalityConstraints: 'Failure: Invalid arguments'
      initalGuess: 'Success'
      status: 'Success'
      parameterValues: [ -2.00000000000279 ]
      outputValue: 20 
    }
    options = {
      algorithm: "NLOPT_LD_TNEWTON_PRECOND_RESTART"
      numberOfParameters:1
      maxObjectiveFunction: objectiveFunc
      inequalityConstraints:[{callback:(()->),tolerance:1}]#should not work for newton
      xToleranceRelative:1e-4
      initalGuess:[-1]
      lowerBounds:[-10]
      upperBounds:[10]
    }
    checkResults(nlopt(options), expectedResult)
    expectedResult.parameterValues[0] = 2
    options.initalGuess[0] = 2.5
    checkResults(nlopt(options), expectedResult)
    options.algorithm = "LN_NELDERMEAD"
    checkResults(nlopt(options), expectedResult)
  )
  it('example', ()->
    myfunc = (n, x, grad)->
      if(grad)
        grad[0] = 0.0
        grad[1] = 0.5 / Math.sqrt(x[1])
      return Math.sqrt(x[1])
    createMyConstraint = (cd)->
      return {
        callback:(n, x, grad)->
          if(grad)
            grad[0] = 3.0 * cd[0] * (cd[0]*x[0] + cd[1]) * (cd[0]*x[0] + cd[1])
            grad[1] = -1.0
          tmp = cd[0]*x[0] + cd[1]
          return tmp * tmp * tmp - x[1]
        tolerance:1e-8
      }
    expectedResult = { 
      minObjectiveFunction: 'Success'
      lowerBounds: 'Success'
      xToleranceRelative: 'Success'
      inequalityConstraints: 'Success'
      initalGuess: 'Success'
      status: 'Success: Optimization stopped because xToleranceRelative or xToleranceAbsolute was reached'
      parameterValues: [ 0.33333333465873644, 0.2962962893886998 ]
      outputValue: 0.5443310476067847 
    }
    options = {
      algorithm: "LD_MMA"
      numberOfParameters:2
      minObjectiveFunction: myfunc
      inequalityConstraints:[createMyConstraint([2.0, 0.0]), createMyConstraint([-1.0, 1.0])]
      xToleranceRelative:1e-4
      initalGuess:[1.234, 5.678]
      lowerBounds:[Number.MIN_VALUE, 0]
    }
    checkResults(nlopt(options), expectedResult)
    options.algorithm = "NLOPT_LN_COBYLA"
    checkResults(nlopt(options), expectedResult)

  )
)