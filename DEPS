turn what is done to UPPERCASE:

libnone: none
libbool: new FALSE TRUE NEGATE OR AND XOR
libint: NEW ADD NEGATE SUBTRACT multiply divide modulo EQUALS greater lesser
liblist: new length add concatenate change(int) omit(int) prepend
         map reduce filter(bool)
         any(bool) all(bool) count(bool,int)
libstr: new(str?) concatenate revert(KILL) get(int) slice(int) equals(bool)
        change(int) replace
        list(list) assemble(list)
        split partition
libbase16: nibblify(str) denibblify(str)

libio: [stdout: print(bool, int, str, list)]

libeval: eval(str)
libobject: set(str?) get(str?) del(str?) size(int) empty(bool)
libmethod: attach(str?) detach(str?)
libcontext: SET(str?)
libload: load import(libcontext,str?)

libfunction: function(str?)
libflow: if(bool) while(bool) for(list)

str? works like this: arg with 1 '__code__' works literally, else as a string
  attach obj name meth  is equivalent to
  attach obj name ( eval 'na' concatenate 'me' ) meth.
