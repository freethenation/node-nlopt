{
  "targets": [
    {
      "target_name": "hello",
      "sources": [ "hello.cc" ],
       "include_dirs": [
	     "./nlopt-2.3/api/"
	   ],
	    "dependencies": [
       		"./nlopt-2.3/binding.gyp:nlopt"
    	]
    }
  ]
}
