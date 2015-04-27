program ma
	type	t1=integer;
			t2=t1;
			t3=record 
				integer x;
				char y, z;
				integer x1  ,x4,x5; #''sdfsdf
				array [1 .. 10]  of integer a;
				#array [1..10]  of char b,c,d;
			end;
			t4=array [5..100]  of char;
			t5=array [1..10]  of integer;
	var		t1 x1,x2,x3;
			t2 x4,x5,x6;
			t3 x7;
			t3 x8,x9;
			t5 x10;
			t4 x11,x12,x13,x14,x15,x16;
			t5 x17,x18,x19;
			char x20;
			char x21,x22;
			integer x23,x24;
			integer x25;
			record 
				integer x;
				char y,z;
				integer x1,x4,x5;
				array [1..10]  of integer a;
				array [1..10]  of char b,c,d;
			end x26,x27,x28;
			record 
				integer x;
				char y,z;
				integer x1,x4,x5;
				array [1..10]  of integer a;
				array [1..10]  of char b,c,d;
			end x29;
			array [1..10]  of integer x30,x31,x32;
			array [1..10]  of char x33,x34,x35,x36;
	procedure L1(var integer a1,a2,a3;var t3 a4,a5;integer a6,a7;var integer a9,a10);
		var t5 x1,x2,x3,x4;
		procedure L2(var integer a1,a3);
			var char x1,x2,x25;
				integer x4,x6;
				t3 x7,x8;
		begin
			x4:=a1;
			if x4 < 0 then
				x6:=9+5;
				x4:=a2+x6
			else
				x4:=9-3;
				x6:=a1*10
			fi;
			write(x4);
			write(x6)
		end
	begin
			L2(x5,x6);
			read(a1){;
			write(x1)} 
	end 
	procedure swap(var integer a,b);
		var integer c;
	begin
		c:=a;
		a:=b;
		b:=c
	end
	procedure helloWorld();
		var array [0..20]  of char  chars;
	begin
		chars[ 0]:='H';
		chars[ 1]:='e';
		chars[ 2]:='l';
		chars[ 3]:='l';
		chars[ 4]:='o';
		chars[ 5]:=',';
		chars[ 6]:=' ';
		chars[ 7]:='W';
		chars[ 8]:='o';
		chars[ 9]:='r';
		chars[10]:='l';
		chars[11]:='d';
		chars[12]:='!';
		chars[13]:=0;
		write(chars)
	end

	procedure gcd(integer a,b;var integer c);
	begin
		if a < b then
			swap(a,b);
			gcd(a,b,c)
		else
			c:=a-(a/b)*b;
			if c=0 then
				c:=b
			else
				a:=b;
				b:=c;
				gcd(a,b,c)
			fi
		fi
	end
begin #main
	x20:='e';
	helloWorld();
	#read(x11);
	#x11[20]:=0;
	#write(x11);
	read(x1);T
	read(x2);
	gcd(x1,x2,x3);
	write(x3);

	#x10:=x19;
	#x10[5]:=10;
	x10[x10[0+5]+10]:=10;
	write(x10[20]);
	#write(x10);
	x20:='e';
	write(x20);
	read(x20);
	write(x20);
	#read(x7.b);
	
	read(x1);
	x2:=x1;
	while 0 < x1 do
		#read(x10[x1]);
		read(x5);
		x10[x1]:=x5;
		x1:=x1-1
	endwh;
	while x1 < x2 do
		write(x10[x1]);
		x1:=x1+1
	endwh
end.