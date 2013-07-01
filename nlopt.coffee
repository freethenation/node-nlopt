_ = require("lodash")
algorithms = [
	"GN_DIRECT",
	"GN_DIRECT_L",
	"GN_DIRECT_L_RAND",
	"GN_DIRECT_NOSCAL",
	"GN_DIRECT_L_NOSCAL",
	"GN_DIRECT_L_RAND_NOSCAL",
	"GN_ORIG_DIRECT",
	"GN_ORIG_DIRECT_L",
	"GD_STOGO",
	"GD_STOGO_RAND",
	"LD_LBFGS_NOCEDAL",
	"LD_LBFGS",
	"LN_PRAXIS",
	"LD_VAR1",
	"LD_VAR2",
	"LD_TNEWTON",
	"LD_TNEWTON_RESTART",
	"LD_TNEWTON_PRECOND",
	"LD_TNEWTON_PRECOND_RESTART",
	"GN_CRS2_LM",
	"GN_MLSL",
	"GD_MLSL",
	"GN_MLSL_LDS",
	"GD_MLSL_LDS",
	"LD_MMA",
	"LN_COBYLA",
	"LN_NEWUOA",
	"LN_NEWUOA_BOUND",
	"LN_NELDERMEAD",
	"LN_SBPLX",
	"LN_AUGLAG",
	"LD_AUGLAG",
	"LN_AUGLAG_EQ",
	"LD_AUGLAG_EQ",
	"LN_BOBYQA",
	"GN_ISRES",
	"AUGLAG",
	"AUGLAG_EQ",
	"G_MLSL",
	"G_MLSL_LDS",
	"LD_SLSQP",
	"LD_CCSAQ"
]

optimize = require('./build/Release/nlopt').optimize
module.exports = (options)->
	#algorithm
	if !options.algorithm then throw "'algorithm' must be specified"
	options.algorithm = _.indexOf(algorithms, options.algorithm.replace("NLOPT_",""))
	if options.algorithm == -1 then throw "unknown or invalid 'algorithm'"

	if !options.skipValidation
		#util functions
		isArrayOfDoubles = (arr)->
			return _.isArray(arr) and _.reduce(arr, ((acc, val)->acc&&_.isNumber(val)), true)
		isArrayOfCallbackTolObjects = (arr)->
			return _.isArray(arr) and _.reduce(arr, ((acc, val)->acc&&_.isObject(val)&&_.isFunction(val.callback)&&_.isNumber(val.tolerance)), true)
		#numberOfParameters
		if !options.numberOfParameters then throw "'numberOfParameters' must be specified"
		if !_.isNumber(options.numberOfParameters) then throw "'numberOfParameters' must be a number"
		#minObjectiveFunction and maxObjectiveFunction
		if !options.minObjectiveFunction and !options.maxObjectiveFunction then throw "'minObjectiveFunction' or 'maxObjectiveFunction' must be specifed"
		if options.minObjectiveFunction and options.maxObjectiveFunction then throw "'minObjectiveFunction' and 'maxObjectiveFunction' should not both be specifed"
		if !_.isFunction(options.minObjectiveFunction || options.maxObjectiveFunction) then throw "'minObjectiveFunction' and 'maxObjectiveFunction' must be functions"
		#lowerBounds
		if options.lowerBounds and !isArrayOfDoubles(options.lowerBounds) then throw "'lowerBounds' should be an array of doubles"
		#upperBounds
		if options.upperBounds and !isArrayOfDoubles(options.upperBounds) then throw "'upperBounds' should be an array of doubles"
		#inequalityConstraints
		if options.inequalityConstraints and !isArrayOfCallbackTolObjects(options.inequalityConstraints) then throw "'inequalityConstraints' should be an array of {callback:function(){}, tolerance:0} objects"
		#equalityConstraints
		if options.equalityConstraints and !isArrayOfCallbackTolObjects(options.equalityConstraints) then throw "'equalityConstraints' should be an array of {callback:function(){}, tolerance:0} objects"
		#initalGuess
		if options.initalGuess and !isArrayOfDoubles(options.initalGuess) then throw "'initalGuess' should be an array of doubles"
		#simple parms
		for parm in ["stopValue", "stopBasedOnObjectiveFunction", "stopBasedOnObjectiveFunctionAbs", "stopBasedOnParameterChange", "stopBasedOnParameterChangeAbs", "stopBasedOnMaxEvals", "stopBasedOnMaxTime"]
			if options[parm] and !_.isNumber(options[parm]) then throw "'#{parm}' must be a double"
	
	#do the optimization
	return optimize(options);
	#ret = optimize(options)
	#return _.pick(ret, ["status", "values", "objectiveValues"])