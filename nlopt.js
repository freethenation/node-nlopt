(function() {
  var algorithms, optimize, _;

  _ = require("lodash");

  algorithms = ["GN_DIRECT", "GN_DIRECT_L", "GN_DIRECT_L_RAND", "GN_DIRECT_NOSCAL", "GN_DIRECT_L_NOSCAL", "GN_DIRECT_L_RAND_NOSCAL", "GN_ORIG_DIRECT", "GN_ORIG_DIRECT_L", "GD_STOGO", "GD_STOGO_RAND", "LD_LBFGS_NOCEDAL", "LD_LBFGS", "LN_PRAXIS", "LD_VAR1", "LD_VAR2", "LD_TNEWTON", "LD_TNEWTON_RESTART", "LD_TNEWTON_PRECOND", "LD_TNEWTON_PRECOND_RESTART", "GN_CRS2_LM", "GN_MLSL", "GD_MLSL", "GN_MLSL_LDS", "GD_MLSL_LDS", "LD_MMA", "LN_COBYLA", "LN_NEWUOA", "LN_NEWUOA_BOUND", "LN_NELDERMEAD", "LN_SBPLX", "LN_AUGLAG", "LD_AUGLAG", "LN_AUGLAG_EQ", "LD_AUGLAG_EQ", "LN_BOBYQA", "GN_ISRES", "AUGLAG", "AUGLAG_EQ", "G_MLSL", "G_MLSL_LDS", "LD_SLSQP", "LD_CCSAQ"];

  optimize = require('./build/Release/nlopt').optimize;

  module.exports = function(options) {
    var isArrayOfCallbackTolObjects, isArrayOfDoubles, parm, _i, _len, _ref;
    options = _.cloneDeep(options);
    if (!options.algorithm) {
      throw "'algorithm' must be specified";
    }
    options.algorithm = _.indexOf(algorithms, options.algorithm.replace("NLOPT_", ""));
    if (options.algorithm === -1) {
      throw "unknown or invalid 'algorithm'";
    }
    if (!options.skipValidation) {
      isArrayOfDoubles = function(arr) {
        return _.isArray(arr) && _.reduce(arr, (function(acc, val) {
          return acc && _.isNumber(val);
        }), true);
      };
      isArrayOfCallbackTolObjects = function(arr) {
        return _.isArray(arr) && _.reduce(arr, (function(acc, val) {
          return acc && _.isObject(val) && _.isFunction(val.callback) && _.isNumber(val.tolerance);
        }), true);
      };
      if (!options.numberOfParameters) {
        throw "'numberOfParameters' must be specified";
      }
      if (!_.isNumber(options.numberOfParameters)) {
        throw "'numberOfParameters' must be a number";
      }
      if (!options.minObjectiveFunction && !options.maxObjectiveFunction) {
        throw "'minObjectiveFunction' or 'maxObjectiveFunction' must be specifed";
      }
      if (options.minObjectiveFunction && options.maxObjectiveFunction) {
        throw "'minObjectiveFunction' and 'maxObjectiveFunction' should not both be specifed";
      }
      if (!_.isFunction(options.minObjectiveFunction || options.maxObjectiveFunction)) {
        throw "'minObjectiveFunction' and 'maxObjectiveFunction' must be functions";
      }
      if (options.lowerBounds && !isArrayOfDoubles(options.lowerBounds)) {
        throw "'lowerBounds' should be an array of doubles";
      }
      if (options.upperBounds && !isArrayOfDoubles(options.upperBounds)) {
        throw "'upperBounds' should be an array of doubles";
      }
      if (options.inequalityConstraints && !isArrayOfCallbackTolObjects(options.inequalityConstraints)) {
        throw "'inequalityConstraints' should be an array of {callback:function(){}, tolerance:0} objects";
      }
      if (options.equalityConstraints && !isArrayOfCallbackTolObjects(options.equalityConstraints)) {
        throw "'equalityConstraints' should be an array of {callback:function(){}, tolerance:0} objects";
      }
      if (options.initalGuess && !isArrayOfDoubles(options.initalGuess)) {
        throw "'initalGuess' should be an array of doubles";
      }
      _ref = ["stopValue", "fToleranceRelative", "fToleranceAbsolute", "xToleranceRelative", "xToleranceAbsolute", "maxEval", "maxTime"];
      for (_i = 0, _len = _ref.length; _i < _len; _i++) {
        parm = _ref[_i];
        if (options[parm] && !_.isNumber(options[parm])) {
          throw "'" + parm + "' must be a double";
        }
      }
    }
    return optimize(options);
  };

}).call(this);
