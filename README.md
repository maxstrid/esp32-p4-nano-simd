# SIMD Testing

This just does a simple SIMD profile on large matrix multiplications.

## Current results

Running this really naive profiling code i get this:

```sh
I (316) main: dspm_mult_f32_ansi: 101 us average
I (316) main: dspm_mult_f32_arp4: 35 us average
```

An interesting note is that when setting the max of my random number float function to FLT_MAX
I get 89us average for `dspm_mult_f32_ansi`, but when I switch to 1000 I get 101us.

I have no clue what the cause for the 10us gap is but the `dspm_mult_f32_arp4` time didn't change at all.
