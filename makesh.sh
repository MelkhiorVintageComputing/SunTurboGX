#!/bin/bash -x

/bin/rm -f setres_alt.sh

/bin/cat pre.sh >> setres_alt.sh

POS=`grep -n TEST_GOES_HERE setres.fth | sed -e 's/:.*//'`
SIZE=`wc -l setres.fth | awk '{ print $1 }'`

/usr/bin/head -n $((POS-1)) setres.fth | grep -v '^(.*)$' >> setres_alt.sh
./gensoa >> setres_alt.sh
/usr/bin/tail -n $((SIZE-POS)) setres.fth | grep -v '^(.*)$' >> setres_alt.sh

/bin/cat post.sh >> setres_alt.sh

chmod a+x setres_alt.sh

scp setres_alt.sh ss20d:
