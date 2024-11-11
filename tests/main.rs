use cvec::CVec;



fn main() {
  unsafe {
    _main();
  }
}

#[allow(invalid_value)]
unsafe fn _main() {
  let mut vec=CVec::<i32>::new();

  fill_vec(&mut vec);
  vec.retain(is_even);

  
  for x in vec.as_ref() {
    print!("{x},");
  }
  println!();
}

#[allow(dead_code)]
unsafe fn print_vec(this: &CVec<i32>,idx: usize) {
  print!("{idx}: ");
  for i in this.as_ref() {
    print!("{i} ");
  }
  println!();
}

unsafe fn fill_vec(this: &mut CVec<i32>) {
  for i in 0..32 {
    this.push(i);
  }
}

extern "C" fn is_even(x: &i32)-> bool {
  x%2==0
}

#[test]
fn _panic_rt_cleanup() {
  std::process::exit(0);
}
