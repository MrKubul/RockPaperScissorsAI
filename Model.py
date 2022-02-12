import numpy as np
import tensorflow as tf

filepath = './rps_model.h5'
trained_model = tf.keras.models.load_model(filepath, compile=True)
class_names = ['None', 'Paper', 'Rock', 'Scissors']


def predict_value(image):
    arr = np.array([image, ])
    prediction = trained_model.predict(arr, batch_size=1)[0]
    max_index_col = np.argmax(prediction, axis=0)
    return round((prediction[max_index_col] / 1) * 100, 3), class_names[max_index_col]
