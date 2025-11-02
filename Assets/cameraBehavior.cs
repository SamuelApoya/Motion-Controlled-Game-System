using UnityEngine;

public class cameraBehavior : MonoBehaviour

{
    public float rotationDegree = 0.1f;
    public float speed = 5.0f;
    // Start is called once before the first execution of Update after the MonoBehaviour is created
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {   
        // rotation controls
        if (Input.GetKey(KeyCode.LeftArrow))
        {
            this.transform.Rotate(0, -rotationDegree, 0, Space.Self); // rotate left
        }

        if (Input.GetKey(KeyCode.RightArrow))
        {
            this.transform.Rotate(0, rotationDegree, 0, Space.Self); // roate right
        }

        if (Input.GetKey(KeyCode.UpArrow))
        {
            this.transform.Rotate(-rotationDegree, 0, 0, Space.Self); // rotate up
        }

        if (Input.GetKey(KeyCode.DownArrow))
        {
            this.transform.Rotate(rotationDegree, 0, 0, Space.Self); // rotate down
        }

        if (Input.GetKey(KeyCode.Z))
        {
            this.transform.Rotate(0, 0, -rotationDegree, Space.Self); // rotate clockwise around z axis
        }

        if (Input.GetKey(KeyCode.Q))
        {
            this.transform.Rotate(0, 0, rotationDegree, Space.Self); // rotate counterclockwise aroun z axis
        }

        // Translation controls
        if (Input.GetKey(KeyCode.W)){
            this.transform.Translate(Vector3.forward * speed * Time.deltaTime, Space.Self); // forward
        }

        if (Input.GetKey(KeyCode.X)){
            this.transform.Translate(Vector3.back * speed * Time.deltaTime, Space.Self); // backward
        }

        if (Input.GetKey(KeyCode.E)){
            this.transform.Translate(Vector3.up * speed * Time.deltaTime, Space.Self); // up
        }

        if (Input.GetKey(KeyCode.C)){
            this.transform.Translate(Vector3.down * speed * Time.deltaTime, Space.Self); // down
        }

        if (Input.GetKey(KeyCode.A)){
            this.transform.Translate(Vector3.left * speed * Time.deltaTime, Space.Self); // left
        }

        if (Input.GetKey(KeyCode.D)){
            this.transform.Translate(Vector3.right * speed * Time.deltaTime, Space.Self); // right
        }

        // Horizontal/Vertical position controls through input axes
        float horizontalInput = Input.GetAxis("Horizontal");
        float verticalInput = Input.GetAxis("Vertical");

        Vector3 movement  = new Vector3(horizontalInput,0 , verticalInput);
        transform.Translate(movement * speed * Time.deltaTime, Space.Self);
        
    }
}
