import numpy as np

a = [ 
    # NO DATA
    ]

f = np.array([[ 0.012339  , -0.02620502, -0.03098542,  0.07781701, -0.0270996 ],
       [ 0.00468379,  0.00022547,  0.06944456, -0.0484358 , -0.02109563],
       [-0.00247619,  0.0222133 , -0.02510456,  0.02109282, -0.01257638],
       [-0.02556051,  0.00968766, -0.01059733, -0.03079028,  0.0493632 ],
       [ 0.01152323, -0.00291923,  0.00064684, -0.01818232,  0.01265076]])

if np.allclose(np.dot(f,a),np.eye(len(f)),atol=1e-05):print(''.join(map(chr,(i for s in a for i in s))))
