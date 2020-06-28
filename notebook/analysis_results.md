---
jupyter:
  jupytext:
    formats: ipynb,md
    text_representation:
      extension: .md
      format_name: markdown
      format_version: '1.2'
      jupytext_version: 1.3.2
  kernelspec:
    display_name: Python 3
    language: python
    name: python3
---

# Geant4 xml analysis

```python
import xml.etree.ElementTree as ET
```

```python
root = ET.parse('../build/detector_loc_000.xml').getroot()
```

```python
for child in root.findall('histogram1d'):
    if child.attrib['name'] == 'h1:0':
        break
print(child.attrib['title'])
xml_axis = child.find('axis')
print(xml_axis.attrib)
```

```python
# get bin 
for it in child.findall('data1d/bin1d'):
    print(it.attrib)
```

# Get 0.662 MeV bin
`/analysis/setFileName detector_loc_009`  
`/analysis/h1/set 0 1 0.65 0.67 MeV`

```python
assert xml_axis.attrib['numberOfBins'] == '1'
assert float(xml_axis.attrib['min']) > 0.6
assert float(xml_axis.attrib['max']) < 0.7
```

```python
count = -1
for it in child.findall('data1d/bin1d'):
    if it.attrib['binNum'] == '0':
        count = int(it.attrib['entries'])
        print('count:', count)
        break
assert count > 0, "couldn't find bin"
```

# Function to extract count

```python
import xml.etree.ElementTree as ET
def get_count_xml(fname, target_mev=0.662, debug=False):
    # parse xml
    root = ET.parse(fname).getroot()
    for child in root.findall('histogram1d'):
        if child.attrib['name'] == 'h1:0':
            break
            
    xml_axis = child.find('axis')
    if debug:
        print(child.attrib['title'])
        print(xml_axis.attrib)
    
    # check target MeV
    assert xml_axis.attrib['numberOfBins'] == '1'
    assert float(xml_axis.attrib['min']) > (target_mev-0.1)
    assert float(xml_axis.attrib['max']) < (target_mev+0.1)
    
    # get count from bin
    count = -1
    for it in child.findall('data1d/bin1d'):
        if it.attrib['binNum'] == '0':
            count = int(it.attrib['entries'])
            if debug:
                print('count:', count)
            break
    assert count > 0, "couldn't find bin"
    
    return count
```

```python
get_count_xml('../build/detector_loc_000.xml')
```

```python
get_count_xml('../build/detector_loc_001.xml', debug=True)
```

# Data visualization

```python
import numpy as np
import matplotlib.pyplot as plt
%matplotlib inline
```

```python
width_len = 21
data = []
for i in range(width_len*width_len):
    fname = f'../build/detector_loc_{i:03}.xml'
    if i%10==0:
        print(fname)
    cnt = get_count_xml(fname)
    data.append(cnt)
```

```python
print(data)
```

```python
viz = np.array(data).reshape(width_len,width_len)
viz = viz.astype(np.float)/viz.max()
plt.imshow(viz)
```

# Localization
## define variables

```python
b_measure = np.array(data)
x_b = np.genfromtxt('../build/detector_position.txt', delimiter=',')
assert x_b.shape[0] == len(b_measure), 'Number of measument should be equal to number of position'
```

```python
# radiation source candidate
x_q = []
MAX_I = 17
MAX_J = 17
factor = 1.5
for j in range(MAX_J):
    for i in range(MAX_I):
        x_pos = factor * (i - (MAX_I - 1) / 2);
        z_pos = factor * (j - (MAX_J - 1) / 2);
        x_q.append([x_pos, 0, z_pos])
x_q = np.array(x_q)
```

```python
# distance matrix
radiation_factor = 100
dist_mat = (x_b[:, np.newaxis]-x_q[np.newaxis])
dist_mat = np.linalg.norm(dist_mat, axis=2)
A = radiation_factor/dist_mat
```

## Optimization

```python
# visualize square image
def imshow(src):
    l = int(np.sqrt(src.shape))
    plt.imshow(src.reshape(l,l))
```

```python
# initial guess for radiation distribution
M = x_q.shape[0]
q_max = 10
q_init = 1/q_max
q = np.array([q_init]*M)
```

```python
imshow(q)
```

```python
b_ave = A.dot(q)
imshow(b_ave)
```

```python
def score_func(q):
    global A, b_measure
    b_ave = A.dot(q)
    score = np.sum(b_measure*np.log(b_ave))-np.sum(b_ave)
    return score

def grad_func(q):
    global A, b_measure
    b_ave = A.dot(q)
    grad_tmp = (b_measure/b_ave)[:,np.newaxis]*A
    grad = grad_tmp.sum(axis=0) - A.sum(axis=0)
    return grad
```

```python
optim_factor = 0.00005
init_score = score_func(q)
print(f'initial score:{init_score}')
```

```python
for i in range(1, 50):
    q_diff = grad_func(q)
    q += optim_factor*q_diff
    
    # bound > 0
    q[q<0]=0
    
    score = score_func(q)
    if i%2==0:
        plt.figure()
        plt.title(f'iter:{i} score:{score} q_max:{q.max():.3}, q_min:{q.min():.3}')
        imshow(q)
```

```python
max_idx = np.argmax(q)
print(f'max intensity location: {x_q[max_idx]}')

```
