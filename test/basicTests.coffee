_ = require('lodash')
expect = require('expect.js')
nlopt = require("../nlopt.js")
request = require("request")

describe('basic', ()->
  before(()->
    
  )
  after(()->
    
  )
  beforeEach(()->
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

    res = nlopt({
      algorithm: "LD_MMA"
      numberOfParameters:2
      minObjectiveFunction: myfunc
      inequalityConstraints:[createMyConstraint([2.0, 0.0]), createMyConstraint([-1.0, 1.0])]
      stopBasedOnParameterChange:1e-4
      initalGuess:[1.234, 5.678]
      lowerBounds:[Number.MIN_VALUE, 0]
    })
    console.log(res)
  )
)