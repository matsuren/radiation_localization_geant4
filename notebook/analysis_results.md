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
width_len = 10
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

```python

```
