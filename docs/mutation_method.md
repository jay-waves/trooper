## Mutate Single ByteArray
```python
if data_size > max_len_:
  decrease size mutation
elif data_size = max_len_:
  same, decrease size mutation
elif data_size < max_len_:
  same, increase, decrease size mutation
```

decrease size mutation:
- erase bytes

same size mutation:
- flip bit
- swap bytes
- change byte
- overwrite from dictionary

increase size:
- insert bytes
- insert from dictionary

