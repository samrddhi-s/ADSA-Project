import streamlit as st
import subprocess
import pandas as pd
# Title of the app
st.title('Rent Finder')

# Collecting location and rent preferences
with st.form(key='rent_preferences_form'):
    st.write("Enter your location and rent preferences:")
    location = st.text_input("Location")
    min_rent = st.number_input("Minimum Rent", min_value=0, step=100)
    max_rent = st.number_input("Maximum Rent", min_value=0, step=100)
    min_bedrooms = st.number_input("Minimum Number of Bedrooms", min_value=0, max_value=10, step=1)
    max_bedrooms = st.number_input("Maximum Number of Bedrooms", min_value=0, max_value=10, step=1)
    submit_rent_button = st.form_submit_button(label='Submit Rent Preferences')

if submit_rent_button:
    command = ['./main', location, str(min_rent), str(max_rent), str(min_bedrooms), str(max_bedrooms)]
    output = subprocess.run(command, capture_output=True, text=True)

    # Display output
    st.write("Output from C++ program:")
    output_lines = output.stdout.strip().split('\n')
    table_data = [line.split(': ') for line in output_lines]
    column_names=["Location", "Name","Rent","Amenities","Number of Bedrooms","Total Score","Blank"]
    df=pd.DataFrame(table_data,columns=column_names)
    df=df.drop(df.columns[-1],axis=1)
    st.dataframe(df)
    

# Optionally, you can add more functionality here, such as searching for rentals based on the provided criteria