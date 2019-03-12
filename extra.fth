

: v19 " 162000000,68067,59,32,396,32,1920,22,10,23,1080,COLOR,0OFFSET" ;
: v20 " 155250000,68092,60,100,60,200,1920,7,7,40,1080,COLOR,0OFFSET" ;
: v21 " 144000000,65454,58,88,44,148,1920,4,5,36,1080,COLOR,0OFFSET" ;


: ics144  0 0 0 a d f f 1 8 4 2 0 5 ;
: ics155  0 0 0 a d f f 1 8 6 4 0 7 ;
: ics162  0 0 0 a d f f 1 8 5 0 0 6 ;

( for 1920x1200x60 )
: ics162  0 0 0 a d f f 1 8 5 0 0 6 ;
: ics162b 0 0 0 a d f f 1 8 6 6 0 7 ;

: ics170b 0 0 0 a d f f 1 b 7 4 3 5 ;

( : r1920x1080x60b " 138375000,66526,60,48,32,80,1920,3,5,23,1080,COLOR,0OFFSET" ; )

( Trying... )
: r1920x1200x60 " 162000000,74175,60,100,32,132,1920,2,6,27,1200,COLOR,0OFFSET" ;

( : hid v19 set-resolution-alt ; )
( : hie v20 set-resolution-alt ; )
( : hif v21 set-resolution-alt ; )
( : hib r1920x1080x60b set-resolution-alt ; )

