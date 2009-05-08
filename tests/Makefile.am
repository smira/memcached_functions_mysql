INCLUDES =
LDADDS = 
LIBS = 

EXTRA_DIST = memc_test.cmp memc_test.sql


record:
	cat memc_test.sql | mysql > memc_test.cmp

test: testapp
	cat memc_test.sql | mysql > memc_test.res
	diff memc_test.cmp memc_test.res
