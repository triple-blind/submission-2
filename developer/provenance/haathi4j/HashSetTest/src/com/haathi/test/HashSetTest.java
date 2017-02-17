package com.haathi.test;

import java.util.HashSet;

public class HashSetTest {
	public static void main(String[] args) {
		HashSet<Long> values = new HashSet<Long>();
		long ev1 = 8304571097496941580L;
		long ev2 = 8304571097496941580L;
		
		values.add(ev1);
		if (values.contains(ev2))
			System.out.println("It worked!");
	}
}
