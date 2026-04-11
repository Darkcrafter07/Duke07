bgame.exe : bgame.obj engine.obj cache1d.obj mmulti.obj a.obj kdmeng.obj k.obj
	wlink f bgame f engine f cache1d f mmulti f a f kdmeng f k
build.exe : build.obj engine.obj cache1d.obj a.obj bstub.obj
	wlink f build f engine f cache1d f a f bstub

bgame.obj : bgame.c build.h names.h pragmas.h
	wcc386 bgame /4r /s /orb
build.obj : build.c build.h pragmas.h
	wcc386 build /4r /s /orb
bstub.obj : bstub.c build.h
	wcc386 bstub /4r /s /orb
cache1d.obj : cache1d.c pragmas.h
	wcc386 cache1d /4r /s /orb
engine.obj : engine.c build.h pragmas.h ves2.h
	wcc386 engine /4r /s /orb
a.obj : a.asm
	wasm a.asm

mmulti.obj : mmulti.c
	wcc386 mmulti /4r /s /or
kdmeng.obj : kdmeng.c makefile.
	wcc386 kdmeng /4r /s /or /d2
k.obj : k.asm makefile.
	wasm k.asm