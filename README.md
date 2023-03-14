# PathTracer

## Bounding Volume Hierarchy


![bvh](imgs/bvh.png)


## Importance Sampling

runtime: 473.7629s

![importance-bunny](imgs/CBbunny_importance_s_64_l_16.png)

runtime: 679.8680s

![uniform-bunny](imgs/CBbunny_uniform_s_64_l_16.png)

<table style="width:100%">
<tr>
<th></th>
<th>Uniform Hemisphere Sampling</th>
<th>Importance Sampling</th>
</tr>

<tr>
<td>s=8

l=4</td>
<td><img src="imgs/CBbunny_uniform_s_8_l_4.png"></td>
<td><img src="imgs/CBbunny_importance_s_8_l_4.png"></td>
</tr>

<tr>
<td>s=64


l=16</td>
<td><img src="imgs/CBbunny_uniform_s_64_l_16.png"></td>
<td><img src="imgs/CBbunny_importance_s_64_l_16.png"></td>
</tr>
</table>

## Global Illumination

<table style="width:100%">
<tr>
<th>m=1</th>
<th>m=2</th>
<th>m=3</th>
</tr>

<tr>
<td><img src="imgs/CBspheres_m_1.png"></td>
<td><img src="imgs/CBspheres_m_2.png"></td>
<td><img src="imgs/CBspheres_m_3.png"></td>

<table style="width:100%">
<tr>
<th>m=4</th>
<th>m=5</th>
<th>m=32</th>
</tr>

<tr>
<td><img src="imgs/CBspheres_m_4.png"></td>
<td><img src="imgs/CBspheres_m_5.png"></td>
<td><img src="imgs/CBspheres_m_32.png"></td>
</tr>
</table>

## Adaptive Sampling

<table style="width:100%">
<tr>
<td><img src="imgs/CBlucy_2048-64-16.png"></td>
<td><img src="imgs/CBlucy_2048-64-16_rate.png"></td>
</tr>
</table>

<table style="width:100%">
<tr>
<td><img src="imgs/CBbunny_2048.png"></td>
<td><img src="imgs/CBbunny_2048-rate.png"></td>
<td><img src="imgs/CBspheres-2048.png"></td>
<td><img src="imgs/CBspheres-2048_rate.png"></td>
</tr>
</table>

## Materials

### Diffuse

<table style="width:100%">
<tr><img src="imgs/CBspheres_diff.png"></tr>
</table>

### Mirror & Glass


<table style="width:100%">
<tr><td><img src="imgs/CBcoil.png"></td></tr>
<tr><td><img src="imgs/CBspheres_m_32.png"></td></tr>
<tr><td><img src="imgs/CBlucy_2048-64-16.png"></td></tr>
</table>

### Microfacet

<table style="width:100%">
<tr>
<th>gold 0.005</th>
<th>gold 0.05</th>
<th>gold 0.25</th>
</tr>
<tr>
<td><img src="imgs/dragon_au_0.005.png"></td>
<td><img src="imgs/dragon_au_0.05.png"></td>
<td><img src="imgs/dragon_au_0.25.png"></td>
</tr>
</table>

<table style="width:100%">
<tr>
<th>gold</th>
<th>silver</th>
<th>zinc</th>
<th>carbon</th>
</tr>
<tr>
<td><img src="imgs/dragon_au.png"></td>
<td><img src="imgs/dragon_ag.png"></td>
<td><img src="imgs/dragon_zn.png"></td>
<td><img src="imgs/dragon_c.png"></td>
</tr>
</table>


## Environment Light

<table style="width:100%">
<tr>
<td><img src="imgs/dragon_env.png"></td>
</tr>
</table>

## Depth of Field

<table style="width:100%">
<tr>
<td><img src="imgs/dragon_depth.png"></td>
</tr>
</table>
