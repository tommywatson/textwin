#
##
#

all: warning 
	$(MAKE) -C src

# check different compilers for warnings/errors in the code
warning:
	# check compilers for warnings
	$(MAKE) -C src warning
	$(MAKE) -C src clean

tilde:
	find ./ -name \*~ | xargs rm -f

clean: tilde
	$(MAKE) -C src clean
	rm -rf $(TOP)/generated
	find ./ -name core | xargs rm -f
	find ./ -name vgcore\* | xargs rm -f
	find ./ -name gmon.out | xargs rm -f

reallyclean: clean

compspecs: 
	echo | gcc -E -dM -

dollars:  # running this always makes me laugh
	sloccount .

commit: clean
	scripts/hook.commit
	git add . 
	git commit
	git push


